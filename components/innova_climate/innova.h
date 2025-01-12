#pragma once

#include "esphome/core/component.h"
#include "esphome/components/modbus/modbus.h"
#include "esphome/components/climate/climate.h"

namespace esphome {
namespace innova {


class Innova : public climate::Climate, public modbus::ModbusDevice, public PollingComponent {
 public:
  void setup() override;
  void loop() override;
  void update() override;
  void dump_config() override;

  climate::ClimateTraits traits() override {
    auto traits = climate::ClimateTraits();
    traits.set_supports_current_temperature(true);
    traits.set_supported_modes({climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_HEAT, climate::CLIMATE_MODE_COOL});
    traits.set_visual_min_temperature(16.0);
    traits.set_visual_max_temperature(28.0);
    traits.set_visual_temperature_step(0.5);
    return traits;
  }

 protected:
  void control(const climate::ClimateCall &call) override;
};

}  // namespace innova
}  // namespace esphome

