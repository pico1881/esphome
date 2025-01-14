// Example: innova.h

#pragma once

#include "esphome/components/modbus/modbus.h"
#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"

namespace esphome {
namespace innova {

class Innova : public esphome::climate::Climate, public PollingComponent, public modbus::ModbusDevice {
 public:
  //Innova() : PollingComponent(15000) {}  // Poll every 15 seconds

  void setup() override;
  void loop() override;
  void dump_config() override;
  void update() override;
  void on_modbus_data(const std::vector<uint8_t> &data) override;
  void write_register(float new_value, uint16_t address);

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

  

 protected:
  int state_{0};
  bool waiting_{false};
  uint32_t last_send_{0};
  bool waiting_for_write_ack_{false};
  float current_temp_{10.0};
  float target_temp_{10.0};
  float water_temp_;
  float fan_speed_;
  float program_;
  float season_;

  void control(const climate::ClimateCall &call) override; /*{
    // Handle climate control actions
    if (call.get_mode().has_value()) {
      // User requested mode change
      this->mode = *call.get_mode();
    }
    if (call.get_target_temperature().has_value()) {
      // User requested target temperature change
      this->target_temperature = *call.get_target_temperature();
    }

    // Apply the changes (e.g., turn on/off relays)
    apply_control();
    this->publish_state();
  }*/


 /*  void apply_control() {
    // Implement logic to control the HVAC system based on the current mode and target temperature
    if (this->mode == climate::CLIMATE_MODE_COOL) {
      // Turn on cooling
    } else if (this->mode == climate::CLIMATE_MODE_HEAT) {
      // Turn on heating
    } else {
      // Turn off HVAC
    }
  }*/
};

}  // namespace innova
}  // namespace esphome
