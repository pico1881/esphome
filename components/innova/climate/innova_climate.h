#pragma once

#include "../innova.h"
#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace innova {

class InnovaClimate : public climate::Climate, public Component {
 public:
  InnovaClimate(Innova *innova) : innova_(innova) {}
  void setup() override;
  void dump_config() override;

 protected:
  Innova *innova_;
  void control(const climate::ClimateCall &call) override;

  /// Return the traits of this controller.
  climate::ClimateTraits traits() override {
    // Return the traits of this climate device.
    auto traits = climate::ClimateTraits();
    traits.set_supports_action(true);
    traits.set_supports_current_temperature(true);
    traits.set_supported_modes({
           climate::CLIMATE_MODE_OFF, 
           climate::ClimateMode::CLIMATE_MODE_HEAT,
           climate::ClimateMode::CLIMATE_MODE_COOL
    });
    traits.set_visual_min_temperature(16.0);
    traits.set_visual_max_temperature(28.0);
    traits.set_visual_target_temperature_step(0.5);
    traits.set_visual_current_temperature_step(0.1);
    traits.set_supported_fan_modes({
            climate::CLIMATE_FAN_AUTO,
            climate::CLIMATE_FAN_LOW,
            climate::CLIMATE_FAN_MEDIUM,
            climate::CLIMATE_FAN_HIGH,
    });
    return traits;
  }

};

}  // namespace innova
}  // namespace esphome
