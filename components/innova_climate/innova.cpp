#include "innova.h"
#include "esphome/core/log.h"

namespace esphome {
namespace innova {

static const char *const TAG = "innova";

using namespace esphome::climate;

void Innova::dump_config() { LOG_CLIMATE("", "Innova BLE Cooker", this); }

void Innova::update() {
    this->current_temperature = 20.0;

    publish_state();
}

}  // namespace innova
}  // namespace esphome

