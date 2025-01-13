#include "innova.h"
#include "esphome/core/log.h"

namespace esphome {
namespace innova {

static const char *const TAG = "innova";
static const uint8_t CMD_READ_REG = 0x03;

void Innova::dump_config() { LOG_CLIMATE("", "Innova Climate", this); }

void Innova::on_modbus_data(const std::vector<uint8_t> &data) {
  auto get_16bit = [&](int i) -> uint16_t { return (uint16_t(data[i * 2]) << 8) | uint16_t(data[i * 2 + 1]); };
  float value = (float) get_16bit(0);
  this->current_temp_ = value;
}

void Innova::update() {
   send(CMD_READ_REG, 0, 2);
   this->current_temperature = current_temp_;
   this->target_temperature = 21.0;
   this->mode = climate::CLIMATE_MODE_HEAT;
   this->action = climate::CLIMATE_ACTION_HEATING;
   this->fan_mode = climate::CLIMATE_FAN_LOW;
   this->publish_state();
}

}  // namespace innova
}  // namespace esphome

