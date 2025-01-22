#include "innova_climate.h"
#include "esphome/core/log.h"

namespace esphome {
namespace innova {
static const char *TAG = "innova.climate";
static const uint8_t CMD_WRITE_REG = 0x06;
static const uint16_t INNOVA_AIR_TEMPERATURE = 0x00;    // reg 0
static const uint16_t INNOVA_WATER_TEMPERATURE = 0x01;  // reg 1
static const uint16_t INNOVA_FAN_SPEED = 0x0F;          // reg 15
static const uint16_t INNOVA_PROGRAM = 0xC9;            // reg 201
static const uint16_t INNOVA_SEASON = 0xE9;             // reg 233
static const uint16_t INNOVA_SETPOINT = 0xE7;           // reg 231

void InnovaClimate::setup() {
    innova_->add_current_temp_callback([this](float state) {
        current_temperature = state;
        publish_state();
    });
    innova_->add_target_temp_callback([this](float state) {
        target_temperature = state;
        publish_state();
    });
    innova_->add_fan_mode_callback([this](int state) {
        climate::ClimateFanMode new_fan_mode;
            switch (state) {
                case 0: new_fan_mode = climate::CLIMATE_FAN_AUTO; break;
                case 1: new_fan_mode = climate::CLIMATE_FAN_MEDIUM; break;
                case 2: new_fan_mode = climate::CLIMATE_FAN_LOW; break;
                case 3: new_fan_mode = climate::CLIMATE_FAN_HIGH; break;
                default: new_fan_mode = climate::CLIMATE_FAN_MEDIUM; break;
            }
            fan_mode = new_fan_mode; 
        publish_state();
    });
	
    innova_->add_climate_mode_callback([this](int state) {
        climate::ClimateMode new_mode;
	switch (state) {
                case 0: new_mode = climate::CLIMATE_MODE_OFF; break;
                case 2: new_mode = climate::CLIMATE_MODE_COOL; break;
                case 3: new_mode = climate::CLIMATE_MODE_HEAT; break;
        	default: new_mode = climate::CLIMATE_MODE_HEAT; break;
        }
            mode = new_mode; 
        publish_state();
    });
	
    innova_->add_climate_action_callback([this](int state) {
        climate::ClimateAction new_action_mode;
	switch (state) {
                case 0: new_action_mode = climate::CLIMATE_ACTION_OFF; break;
                case 2: new_action_mode = climate::CLIMATE_ACTION_COOLING; break;
                case 3: new_action_mode = climate::CLIMATE_ACTION_HEATING; break;
		case 4: new_action_mode = climate::CLIMATE_ACTION_IDLE; break;
        	default: new_action_mode = climate::CLIMATE_ACTION_IDLE; break;
        }
            action = new_action_mode; 
        publish_state();
    });  
}

void InnovaClimate::control(const climate::ClimateCall &call) {
    if (call.get_mode().has_value()) {
        this->mode = *call.get_mode();
	    climate::ClimateMode set_mode = *call.get_mode();
	    int mode;
	    switch (set_mode) {
		    case climate::CLIMATE_MODE_OFF: mode = 1; break;
		    case climate::CLIMATE_MODE_HEAT: mode = 2; break;
		    case climate::CLIMATE_MODE_COOL: mode = 3; break;
		    default: mode = 2; break;
	    }
        innova_->set_clima_mode(mode);  
        ESP_LOGD(TAG, "Operation mode changed to: %i", mode);
    }

    if (call.get_fan_mode().has_value()) {
        this->fan_mode = *call.get_fan_mode();
	    int mode;
	    switch (fan_mode.value()) {
		    case climate::CLIMATE_FAN_LOW: mode = 2; break;
		    case climate::CLIMATE_FAN_MEDIUM: mode = 1; break;
		    case climate::CLIMATE_FAN_HIGH: mode = 3; break;
		    case climate::CLIMATE_FAN_AUTO: mode = 0; break;
		    default: mode = 1; break;
	    }
        innova_->set_fan_mode(mode);       
        ESP_LOGD(TAG, "Fan mode set to: %i", fan_mode);
    }
    
    if (call.get_target_temperature().has_value()) {
        this->target_temperature = *call.get_target_temperature();
        ESP_LOGD(TAG, "Target temperature changed to: %f", target_temperature);
        float target = *call.get_target_temperature() * 10.0;
	innova_->set_target_temp(target);
    }
    
    this->publish_state();
}

void InnovaClimate::dump_config() { 
    LOG_CLIMATE("", "Innova Climate", this); 
}

}  // namespace innova
}  // namespace esphome
