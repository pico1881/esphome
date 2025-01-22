#include "innova.h"
#include "esphome/core/log.h"
#include "const.h"

namespace esphome {
namespace innova {

static const char *const TAG = "innova";

static const uint16_t REGISTER[] = {INNOVA_AIR_TEMPERATURE, INNOVA_WATER_TEMPERATURE, INNOVA_FAN_SPEED, INNOVA_PROGRAM, INNOVA_SEASON, INNOVA_SETPOINT};

void INNOVA::on_modbus_data(const std::vector<uint8_t> &data) {
  auto get_16bit = [&](int i) -> uint16_t { return (uint16_t(data[i * 2]) << 8) | uint16_t(data[i * 2 + 1]); };
  
  this->waiting_ = false;
//  ESP_LOGV(TAG, "Data: %s", hexencode(data).c_str());   
   float value = (float) get_16bit(0);
//   for (int i = 0; i < 2; i++)
 //    value /= 10.0;
   switch (this->state_) {
     case 1:
       value /= 10.0;
       ESP_LOGD(TAG, "Air=%.1f", value);
       if (this->air_temperature_sensor_ != nullptr)
         this->air_temperature_sensor_->publish_state(value);
       break;
     case 2:
       value /= 10.0;
       ESP_LOGD(TAG, "Water=%.1f", value);
       if (this->water_temperature_sensor_ != nullptr)
         this->water_temperature_sensor_->publish_state(value);
       break;
     case 3:
       ESP_LOGD(TAG, "FAN=%.1f", value);
       if (this->fan_speed_sensor_ != nullptr)
         this->fan_speed_sensor_->publish_state(value);
       break;
     case 4:
       ESP_LOGD(TAG, "PRG=%.1f", value);
       if (this->program_sensor_ != nullptr)
         this->program_sensor_->publish_state(value);
       break;
     case 5:
       ESP_LOGD(TAG, "Season=%.1f", value);
       if (this->season_sensor_ != nullptr)
         this->season_sensor_->publish_state(value);
       break;
     case 6:
       value /= 10.0;
       ESP_LOGD(TAG, "Setpoint=%.1f", value);
       if (this->setpoint_sensor_ != nullptr)
         this->setpoint_sensor_->publish_state(value);
       break;
   }
   if (++this->state_ > 6)
     this->state_ = 0;
   
}

 void INNOVA::loop() {
   uint32_t now = millis();
   // timeout after 15 seconds
   if (this->waiting_ && (now - this->last_send_ > 15000)) {
     ESP_LOGW(TAG, "timed out waiting for response");
     this->waiting_ = false;
   }
   if (this->waiting_ || (this->state_ == 0))
     return;
   this->last_send_ = now;
   send(CMD_READ_REG, REGISTER[this->state_ - 1], 2);
   this->waiting_ = true;
 }


void INNOVA::update() { this->state_ = 1; }
void INNOVA::dump_config() {
  ESP_LOGCONFIG(TAG, "INNOVA:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("", "Air Temperature", this->air_temperature_sensor_);
  LOG_SENSOR("", "Water Temperature", this->water_temperature_sensor_);
  LOG_SENSOR("", "FAN speed", this->fan_speed_sensor_);
  LOG_SENSOR("", "Program", this->program_sensor_);
  LOG_SENSOR("", "Season", this->season_sensor_);
  LOG_SENSOR("", "Setpoint", this->setpoint_sensor_);   
}


}  // namespace innova
}  // namespace esphome
