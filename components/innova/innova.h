#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/modbus/modbus.h"

namespace esphome {
namespace innova {


class INNOVA : public PollingComponent, public modbus::ModbusDevice {
 public:
  void set_air_temperature_sensor(sensor::Sensor *air_temperature_sensor) { air_temperature_sensor_ = air_temperature_sensor; }
  void set_water_temperature_sensor(sensor::Sensor *water_temperature_sensor) { water_temperature_sensor_ = water_temperature_sensor; }
  void set_fan_speed_sensor(sensor::Sensor *fan_speed_sensor) { fan_speed_sensor_ = fan_speed_sensor; }
  void set_program_sensor(sensor::Sensor *program_sensor) { program_sensor_ = program_sensor; }
  void set_season_sensor(sensor::Sensor *season_sensor) { season_sensor_ = season_sensor; }
  void set_setpoint_sensor(sensor::Sensor *setpoint_sensor) { setpoint_sensor_ = setpoint_sensor; }

  void loop() override;
  void update() override;

  void on_modbus_data(const std::vector<uint8_t> &data) override;

  void dump_config() override;

 protected:
  int state_{0};
  bool waiting_{false};
  uint32_t last_send_{0};

  sensor::Sensor *air_temperature_sensor_{nullptr};
  sensor::Sensor *water_temperature_sensor_{nullptr};
  sensor::Sensor *fan_speed_sensor_{nullptr};
  sensor::Sensor *program_sensor_{nullptr};
  sensor::Sensor *season_sensor_{nullptr};
  sensor::Sensor *setpoint_sensor_{nullptr};
};


}  // namespace innova
}  // namespace esphome
