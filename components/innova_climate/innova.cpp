#include "innova.h"
#include "esphome/core/log.h"

namespace esphome {
namespace innova {

static const char *const TAG = "innova";

void Innova::dump_config() { LOG_CLIMATE("", "Innova Climate", this); }


void Innova::update() {
   this->current_temperature = 20.0;
   this->target_temperature = 21.0;
   this->mode = climate::CLIMATE_MODE_HEAT;
   this->action = climate::CLIMATE_ACTION_HEATING;
   this->fan_mode = climate::CLIMATE_FAN_MEDIUM;
   this->publish_state();
}

}  // namespace innova
}  // namespace esphome

