#include "innova.h"
#include "esphome/core/log.h"

namespace esphome {
namespace innova {

static const char *const TAG = "innova";
static const uint8_t CMD_READ_REG = 0x03;
static const uint8_t CMD_WRITE_REG = 0x06;
static const uint16_t INNOVA_AIR_TEMPERATURE = 0x00;    // reg 0
static const uint16_t INNOVA_WATER_TEMPERATURE = 0x01;  // reg 1
static const uint16_t INNOVA_FAN_SPEED = 0x0F;          // reg 15
static const uint16_t INNOVA_PROGRAM = 0xC9;            // reg 201
static const uint16_t INNOVA_SEASON = 0xE9;             // reg 233
static const uint16_t INNOVA_SETPOINT = 0xE7;           // reg 231
static const uint16_t REGISTER[] = {INNOVA_AIR_TEMPERATURE, INNOVA_SETPOINT, INNOVA_FAN_SPEED, INNOVA_PROGRAM, INNOVA_SEASON, INNOVA_WATER_TEMPERATURE};

void Innova::setup() {
    this->current_temperature = 20.0;
    this->target_temperature = 21.0;
    //this->mode = climate::CLIMATE_MODE_HEAT;
    //this->action = climate::CLIMATE_ACTION_HEATING;
    //this->fan_mode = climate::CLIMATE_FAN_LOW;
    this->publish_state();
}

void Innova::on_modbus_data(const std::vector<uint8_t> &data) {
    this->waiting_ = false;
    if (data.size() < 2) {
        ESP_LOGW(TAG, "Invalid data packet size (%d) for state %d", data.size(), this->state_);
        return;
    }
    ESP_LOGD(TAG, "Data: %s", format_hex_pretty(data).c_str());
    read_loop(data);
}

void Innova::read_loop(const std::vector<uint8_t> &data) {
    auto get_16bit = [&](int i) -> uint16_t { return (uint16_t(data[i * 2]) << 8) | uint16_t(data[i * 2 + 1]); };

    float value = (float) get_16bit(0);

    switch (this->state_) {
        case 1:
            value /= 10.0;
            this->current_temp_ = value;
            this->current_temperature = value;
            ESP_LOGD(TAG, "Air temperature=%.1f", this->current_temp_);
            this->publish_state();
        break;
        case 2:
            value /= 10.0;
            this->target_temp_ = value;
            this->target_temperature = value;    
            ESP_LOGD(TAG, "Setpoint temperature=%.1f", this->target_temp_);
            this->publish_state();
        break;
        case 3:
            this->fan_speed_ = (int)value;   
            ESP_LOGD(TAG, "Fan speed=%d", this->fan_speed_);
            this->publish_state();
        break;
        case 4:
            this->program_ = (int)value;   
            climate::ClimateFanMode fmode;
            switch ((int) value & 0b111) {
                case 0: fmode = climate::CLIMATE_FAN_AUTO; break;
                case 1: fmode = climate::CLIMATE_FAN_MEDIUM; break;
                case 2: fmode = climate::CLIMATE_FAN_LOW; break;
                case 3: fmode = climate::CLIMATE_FAN_HIGH; break;
                default: fmode = climate::CLIMATE_FAN_MEDIUM; break;
            }
            this->fan_mode = fmode;    
            ESP_LOGD(TAG, "Program=%d", this->program_);
            this->publish_state();
        break;
        case 5:
            this->season_ = (int)value;   
            climate::ClimateMode smode;
            switch ((int)value) {
                case 3: smode = climate::CLIMATE_MODE_HEAT; break;
                case 5: smode = climate::CLIMATE_MODE_COOL; break;
                default: smode = climate::CLIMATE_MODE_OFF; break;
            }
            this->mode = smode; 
            if (this->season_ == 3 && this->fan_speed_ > 0) {
                this->action = climate::CLIMATE_ACTION_HEATING;
            } else if (this->season_ == 5 && this->fan_speed_ > 0) {
                 this->action = climate::CLIMATE_ACTION_COOLING;
            } else if (this->program_ & (1<<7)) {
                this->action = climate::CLIMATE_ACTION_OFF;	
            } else {
                this->action = climate::CLIMATE_ACTION_IDLE;  
            }
            ESP_LOGD(TAG, "Season=%d", this->season_);
            this->publish_state();
        break;
        case 6:
            value /= 10.0;
            this->water_temp_ = value;   
            ESP_LOGD(TAG, "Water temperature=%.1f", this->water_temp_);
            this->publish_state();
        break;
    }
    if (++this->state_ > 6)
        this->state_ = 0;

}
void Innova::loop() {
    uint32_t now = millis();

    // timeout after 15 seconds
    if (this->waiting_ && (now - this->last_send_ > 15000)) {
        ESP_LOGW(TAG, "timed out waiting for response");
        this->waiting_ = false;
    }

    if (this->waiting_ || (this->state_ == 0)) return;

    this->last_send_ = now;
    this->waiting_ = true;	

    send(CMD_READ_REG, REGISTER[this->state_ - 1], 1);
}

void Innova::update() {
    this->state_ = 1;
}

void Innova::control(const climate::ClimateCall &call) {
    if (call.get_mode().has_value()) {
        int curr_prg = this->program_;
        int new_prg = curr_prg;
        this->mode = *call.get_mode();
        climate::ClimateMode mode = *call.get_mode();
        switch (mode) {
            case climate::CLIMATE_MODE_OFF:
                ESP_LOGD(TAG, "Set Climate Mode: OFF");
                new_prg = curr_prg | (1 << 7);
                write_register(CMD_WRITE_REG,new_prg, INNOVA_PROGRAM);
            break;
            case climate::CLIMATE_MODE_HEAT:
                ESP_LOGD(TAG, "Set Climate Mode: HEAT");
                write_register(CMD_WRITE_REG,3, INNOVA_SEASON);
                new_prg = curr_prg & ~(1 << 7);  
                //write_register(CMD_WRITE_REG,new_prg, INNOVA_PROGRAM);
            break;
            case climate::CLIMATE_MODE_COOL:
                ESP_LOGD(TAG, "Set Climate Mode:COOL");
                write_register(CMD_WRITE_REG,5, INNOVA_SEASON);
                new_prg = curr_prg & ~(1 << 7);
                //write_register(CMD_WRITE_REG,new_prg, INNOVA_PROGRAM);
            break;
            default: 
                ESP_LOGW(TAG, "Unsupported mode: %d", mode); 
            break;
        }
    }

    if (call.get_fan_mode().has_value()) {
        int mode;
        this->fan_mode = *call.get_fan_mode();
        switch (fan_mode.value()) {
            case climate::CLIMATE_FAN_LOW: mode = 2; break;
            case climate::CLIMATE_FAN_MEDIUM: mode = 1; break;
            case climate::CLIMATE_FAN_HIGH: mode = 3; break;
            case climate::CLIMATE_FAN_AUTO: mode = 0; break;
            default: mode = 2; break;
        }
        ESP_LOGD(TAG, "Fan mode set to: %i", mode);
        write_register(CMD_WRITE_REG,mode, INNOVA_PROGRAM);
    }
    
    if (call.get_target_temperature().has_value()) {
        // User requested target temperature change
        this->target_temperature = *call.get_target_temperature();
        float target = *call.get_target_temperature() * 10.0;
        ESP_LOGD(TAG, "Set Target=%.1f", target);
        write_register(CMD_WRITE_REG,target, INNOVA_SETPOINT);
    }
    this->publish_state();
}

void Innova::write_register(uint8_t function, float new_value, uint16_t address)
{
    WriteableData data;
    data.function_value = function;
    data.write_value = new_value;
    data.register_value = address;
    writequeue_.emplace_back(data);
    ESP_LOGD(TAG, "Data write pending: function (%i), value (%i), address (%i)", data.function_value, data.write_value, data.register_value);
    
    uint8_t payload[] = {(uint8_t)(data.write_value >> 8), (uint8_t)data.write_value };
    send( data.function_value,data.register_value,1,sizeof(payload),payload);

    //uint16_t value_to_write = new_value;
    //uint8_t payload[] = {(uint8_t)(value_to_write >> 8), (uint8_t)value_to_write };
    //waiting_for_write_ack_= true;
    //send(CMD_WRITE_REG,address,1,sizeof(payload),payload);
}

void Innova::dump_config() { 
    //LOG_CLIMATE("", "Innova Climate", this); 
    ESP_LOGCONFIG(TAG, "INNOVA:");
    ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
}

}  // namespace innova
}  // namespace esphome
