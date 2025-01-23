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
static const uint16_t INNOVA_OUT = 0x09;           	// reg 9
static const uint16_t REGISTER[] = {INNOVA_AIR_TEMPERATURE, INNOVA_SETPOINT, INNOVA_FAN_SPEED, INNOVA_PROGRAM, INNOVA_SEASON, INNOVA_WATER_TEMPERATURE, INNOVA_OUT};


void Innova::add_current_temp_callback(std::function<void(float)> &&callback) { current_temp_callback_.add(std::move(callback)); }
void Innova::add_target_temp_callback(std::function<void(float)> &&callback) { target_temp_callback_.add(std::move(callback)); }
void Innova::add_fan_mode_callback(std::function<void(int)> &&callback) { fan_mode_callback_.add(std::move(callback)); }
void Innova::add_climate_mode_callback(std::function<void(int)> &&callback) { climate_mode_callback_.add(std::move(callback)); }
void Innova::add_climate_action_callback(std::function<void(int)> &&callback) { climate_action_callback_.add(std::move(callback)); }
void Innova::add_key_lock_callback(std::function<void(bool)> &&callback) { key_lock_callback_.add(std::move(callback)); }

void Innova::on_modbus_data(const std::vector<uint8_t> &data) {
    this->waiting_ = false;
    if (!waiting_for_write_ack_ && data.size() < 2) {
        ESP_LOGW(TAG, "Invalid data packet size (%d) for state %d", data.size(), this->state_);
        return;
    }
    //ESP_LOGD(TAG, "Data: %s", format_hex_pretty(data).c_str());
	//  Command response is 4 bytes echoing the write command
	if (waiting_for_write_ack_ )  {
		waiting_for_write_ack_ = false ; 
		if (data.size() == 4) {
			ESP_LOGD(TAG, "Write command succeeded");
		} else {
			ESP_LOGW(TAG, "Invalid data packet size (%d) while waiting for write command response", data.size());
		}
		return ; 
    }
    

    auto get_16bit = [&](int i) -> uint16_t { return (uint16_t(data[i * 2]) << 8) | uint16_t(data[i * 2 + 1]); };

    int value = get_16bit(0);
    float f_value = (float) get_16bit(0);

    switch (this->state_) {
        case 1:
            f_value /= 10.0;
            this->current_temp_ = f_value;
	        if (this->air_temperature_sensor_ != nullptr)
               this->air_temperature_sensor_->publish_state(f_value);
	        current_temp_callback_.call(current_temp_);
            //ESP_LOGD(TAG, "Current temperature=%.1f", this->current_temp_);
        break;
        case 2:
            f_value /= 10.0;
            this->target_temp_ = f_value;
            if (this->setpoint_sensor_ != nullptr)
                 this->setpoint_sensor_->publish_state(f_value);
 	        target_temp_callback_.call(target_temp_);
            //ESP_LOGD(TAG, "Target temperature=%.1f", this->target_temp_);
        break;
        case 3:
            this->fan_speed_ = value;   
            if (this->fan_speed_sensor_ != nullptr)
                this->fan_speed_sensor_->publish_state(value);            
          //  ESP_LOGD(TAG, "Fan speed=%d", this->fan_speed_);
        break;
        case 4:
            this->program_ = value;     
            if (this->program_sensor_ != nullptr)
                this->program_sensor_->publish_state(value);
            fan_mode_callback_.call(value & 0b111);  
		
            if ((this->program_ & (0x0010)) == 16) {
                key_lock_callback_.call(true);
            } else {
		key_lock_callback_.call(false);
	    }
		    
	   // ESP_LOGD(TAG, "Program=%d", this->program_);
        break;
        case 5:
            this->season_ = value;   
            if (this->season_sensor_ != nullptr)
                this->season_sensor_->publish_state(value);
		
            if (this->season_ == 3 && !((this->program_ & (0x0080)) == 128)) {
                climate_mode_callback_.call(3);
            } else if (this->season_ == 5 && !((this->program_ & (0x0080)) == 128)) {
                climate_mode_callback_.call(2);
            } else {
                climate_mode_callback_.call(0);
            }
            
            if (this->season_ == 3 && this->fan_speed_ > 0) {
                climate_action_callback_.call(3);	//heating
            } else if (this->season_ == 5 && this->fan_speed_ > 0) {
                climate_action_callback_.call(2);	//cooling
            } else if ((this->program_ & (0x0080)) == 128) {
                climate_action_callback_.call(0);	//off
            } else {
                climate_action_callback_.call(4);	//idle
            }
	
           // ESP_LOGD(TAG, "Season=%d", this->season_);
        break;
        case 6:
            f_value /= 10.0;
            this->water_temp_ = f_value;  
            if (this->water_temperature_sensor_ != nullptr)
                this->water_temperature_sensor_->publish_state(f_value);
          //  ESP_LOGD(TAG, "Water temperature=%.1f", this->water_temp_);
        break;
        case 7:
            this->out_reg_ = value;   
            if (this->out_reg_sensor_ != nullptr) 
                this->out_reg_sensor_->publish_state(value);  
            
            if (this->boiler_relay_sensor_ != nullptr) {
                if ((this->out_reg_ & (0x0008)) ) {
                    this->boiler_relay_sensor_->publish_state(true); 
                } else {
                    this->boiler_relay_sensor_->publish_state(false); 
                }
            }
	
            if (this->chiller_relay_sensor_ != nullptr) {
                if ((this->out_reg_ & (0x0004)) ) {
                    this->chiller_relay_sensor_->publish_state(true); 
                } else {
                    this->chiller_relay_sensor_->publish_state(false); 
                }
            }
		
           // ESP_LOGD(TAG, "Output=%d", this->out_reg_);
        break;
    }
    if (++this->state_ > 7){
        this->state_ = 0;
    	//this->publish_state();
    }
}
void Innova::loop() {
    uint32_t now = millis();

    // timeout after 15 seconds
    if (this->waiting_ && (now - this->last_send_ > 15000)) {
        ESP_LOGW(TAG, "Timed out waiting for response");
        this->waiting_ = false;
    }

	
    if (this->waiting_ || (this->state_ == 0) || (now - this->last_send_ < 100)) return;

    //ESP_LOGD(TAG, "State=%d", this->state_);

    if (this->writequeue_.size() > 0) {
      //  ESP_LOGD(TAG, "Write mode: Write queue size is now: %d",this->writequeue_.size());
        writeModbusRegister(this->writequeue_.front());
        this->writequeue_.pop_front();
    } else {
	//this-write_data_ == false;
        send(CMD_READ_REG, REGISTER[this->state_ - 1], 1);        
    }
    
    this->last_send_ = now;
    this->waiting_ = true;	
}

void Innova::update() {
    this->state_ = 1;
}

void Innova::add_to_queue(uint8_t function, float new_value, uint16_t address) {
    WriteableData data;
    data.function_value = function;
    data.write_value = new_value;
    data.register_value = address;
    writequeue_.emplace_back(data);
   // ESP_LOGD(TAG, "Data write pending: function (%i), value (%i), address (%i)", data.function_value, data.write_value, data.register_value);
}

void Innova::writeModbusRegister(WriteableData write_data) { 
    uint8_t payload[] = {(uint8_t)(write_data.write_value >> 8), (uint8_t)write_data.write_value };
    send( write_data.function_value,write_data.register_value,1,sizeof(payload),payload);
    this->waiting_for_write_ack_ = true ;
}

void Innova::set_target_temp(int target_temp) {
        add_to_queue(CMD_WRITE_REG, target_temp, INNOVA_SETPOINT);
	this->state_ = 1; //force modbus update
}

void Innova::set_fan_mode(int fan_value) {
        int curr_prg = this->program_;
        int new_prg = curr_prg;
        switch (fan_value) {
            case 0: new_prg = (curr_prg & ~(0b111)); break;	
            case 1: new_prg = (curr_prg & ~(0b111)) | 1; break;		
            case 2: new_prg = (curr_prg & ~(0b111)) | 2; break;
            case 3: new_prg = (curr_prg & ~(0b111)) | 3;  break;
            default: new_prg = (curr_prg & ~(0b111)) | 1; break;
        }
        add_to_queue(CMD_WRITE_REG, new_prg, INNOVA_PROGRAM);
	this->state_ = 1; //force modbus update
}

void Innova::set_key_lock(bool key_lock) {
        int curr_prg = this->program_;
        int new_prg = curr_prg;	
   	if (key_lock) {
		new_prg = curr_prg | (1 << 4);  
		add_to_queue(CMD_WRITE_REG,new_prg, INNOVA_PROGRAM);
   	} else {
                new_prg = curr_prg & ~(1 << 4);  
                add_to_queue(CMD_WRITE_REG,new_prg, INNOVA_PROGRAM);
    	}
	
	this->state_ = 1; //force modbus update
}

void Innova::set_clima_mode(int mode_value) {
        int curr_prg = this->program_;
        int new_prg = curr_prg;
        switch (mode_value) {
            case 1: 
		new_prg = curr_prg | (1 << 7);  
		add_to_queue(CMD_WRITE_REG,new_prg, INNOVA_PROGRAM);
	    break;	
            case 2: 
                add_to_queue(CMD_WRITE_REG,3, INNOVA_SEASON);
                new_prg = curr_prg & ~(1 << 7);  
                add_to_queue(CMD_WRITE_REG,new_prg, INNOVA_PROGRAM);
            break;		
            case 3: 
                add_to_queue(CMD_WRITE_REG,5, INNOVA_SEASON);
                new_prg = curr_prg & ~(1 << 7);
                add_to_queue(CMD_WRITE_REG,new_prg, INNOVA_PROGRAM);
            break;

            default:
            break;
        }

	    this->state_ = 1; //force modbus update
}

void Innova::dump_config() { 
  //  LOG_CLIMATE("", "Innova Climate", this); 
    ESP_LOGCONFIG(TAG, "INNOVA:");
    ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
    LOG_SENSOR("", "Air Temperature", this->air_temperature_sensor_);
    LOG_SENSOR("", "Water Temperature", this->water_temperature_sensor_);
    LOG_SENSOR("", "FAN speed", this->fan_speed_sensor_);
    LOG_SENSOR("", "Program register", this->program_sensor_);
    LOG_SENSOR("", "Season", this->season_sensor_);
    LOG_SENSOR("", "Setpoint", this->setpoint_sensor_);  
    LOG_SENSOR("", "Output register", this->out_reg_sensor_);  
    LOG_BINARY_SENSOR("", "Boiler relay", this->boiler_relay_sensor_);  
    LOG_BINARY_SENSOR("", "Chiller relay", this->chiller_relay_sensor_);  	
}

}  // namespace innova
}  // namespace esphome
