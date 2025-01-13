#include "innova.h"
#include "esphome/core/log.h"

namespace esphome {
namespace innova {

static const char *const TAG = "innova";
static const uint8_t CMD_READ_REG = 0x03;
static const uint16_t INNOVA_AIR_TEMPERATURE = 0x00; // reg 0
static const uint16_t INNOVA_WATER_TEMPERATURE = 0x01; // reg 1
static const uint16_t INNOVA_FAN_SPEED = 0x0F; // reg 15
static const uint16_t INNOVA_PROGRAM = 0xC9;  // reg 201
static const uint16_t INNOVA_SEASON = 0xE9;  // reg 233
static const uint16_t INNOVA_SETPOINT = 0xE7; // reg 231

void Innova::setup() {
   this->current_temperature = 20.0;
   this->target_temperature = 21.0;
   this->mode = climate::CLIMATE_MODE_HEAT;
   this->action = climate::CLIMATE_ACTION_HEATING;
   this->fan_mode = climate::CLIMATE_FAN_LOW;
   this->publish_state();
}

void Innova::on_modbus_data(const std::vector<uint8_t> &data) {
  auto get_16bit = [&](int i) -> uint16_t { return (uint16_t(data[i * 2]) << 8) | uint16_t(data[i * 2 + 1]); };
//  this->waiting_ = false;
  float value = (float) get_16bit(0);
  value /= 10.0;
  this->current_temp_ = value;
  //this->current_temperature = value;
  //this->publish_state();
  ESP_LOGD(TAG, "Air=%.1f", value);
}

 void Innova::loop() {
//   uint32_t now = millis();
   // timeout after 15 seconds
//   if (this->waiting_ && (now - this->last_send_ > 15000)) {
//     ESP_LOGW(TAG, "timed out waiting for response");
//     this->waiting_ = false;
//   }
//   if (this->waiting_ )
//     return;
//   this->last_send_ = now;
//   send(CMD_READ_REG, INNOVA_AIR_TEMPERATURE, 2);
//   this->waiting_ = true;
 }

void Innova::update() {
   send(CMD_READ_REG, 0, 2);
   this->current_temperature = this->current_temp_;
   this->publish_state();
}

void Innova::control(const climate::ClimateCall &call) {
    // Handle climate control actions
    if (call.get_mode().has_value()) {
      // User requested mode change
      this->mode = *call.get_mode();
    }
    if (call.get_target_temperature().has_value()) {
      // User requested target temperature change
      this->target_temperature = *call.get_target_temperature();
    }
    this->publish_state();
  }

void Innova::dump_config() { 
  LOG_CLIMATE("", "Innova Climate", this); 
  ESP_LOGCONFIG(TAG, "INNOVA:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
}

}  // namespace innova
}  // namespace esphome

