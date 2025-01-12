#include "innova.h"
#include "esphome/core/log.h"

namespace esphome {
namespace innova {

static const char *const TAG = "innova";

using namespace esphome::climate;

  void Innova::dump_config() { 
    LOG_CLIMATE("", "Innova climate", this); 
  }

  void Innova::update() {
    // This will be called every "update_interval" milliseconds.
    // Poll the sensor and update the climate state.
    float temperature = read_temperature();
    float humidity = read_humidity();

    // Publish updated values
    this->publish_state();
  }


}  // namespace innova
}  // namespace esphome
