#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/modbus/modbus.h"
#include "esphome/core/helpers.h"
#include <deque>

namespace esphome {
namespace innova {

struct WriteableData
{
  uint8_t function_value;
  uint16_t register_value;
  uint16_t write_value;
};

class Innova : public PollingComponent, public modbus::ModbusDevice {
 public:
  void set_air_temperature_sensor(sensor::Sensor *air_temperature_sensor) { air_temperature_sensor_ = air_temperature_sensor; }
  void set_water_temperature_sensor(sensor::Sensor *water_temperature_sensor) { water_temperature_sensor_ = water_temperature_sensor; }
  void set_fan_speed_sensor(sensor::Sensor *fan_speed_sensor) { fan_speed_sensor_ = fan_speed_sensor; }
  void set_program_sensor(sensor::Sensor *program_sensor) { program_sensor_ = program_sensor; }
  void set_season_sensor(sensor::Sensor *season_sensor) { season_sensor_ = season_sensor; }
  void set_setpoint_sensor(sensor::Sensor *setpoint_sensor) { setpoint_sensor_ = setpoint_sensor; }
  void set_out_reg_sensor(sensor::Sensor *out_reg_sensor) { out_reg_sensor_ = out_reg_sensor; }
  void set_boiler_relay_sensor(binary_sensor::BinarySensor *boiler_relay_sensor) { boiler_relay_sensor_ = boiler_relay_sensor; }
  void set_chiller_relay_sensor(binary_sensor::BinarySensor *chiller_relay_sensor) { chiller_relay_sensor_ = chiller_relay_sensor; }

  void add_target_temp_callback(std::function<void(float)> &&callback);
  void add_current_temp_callback(std::function<void(float)> &&callback);
  void add_fan_mode_callback(std::function<void(int)> &&callback);
  void add_climate_mode_callback(std::function<void(int)> &&callback);
  void add_climate_action_callback(std::function<void(int)> &&callback);
  void add_key_lock_callback(std::function<void(bool)> &&callback);

  void loop() override;
  void dump_config() override;
  void update() override;
  void on_modbus_data(const std::vector<uint8_t> &data) override;
  void add_to_queue(uint8_t function, float new_value, uint16_t address);
  void writeModbusRegister(WriteableData write_data);

  void set_fan_mode(int fan_value);
  void set_clima_mode(int mode_value);
  void set_target_temp(int target_temp);
  void set_key_lock(bool key_lock);

  float current_temp_;
  float target_temp_;
  float water_temp_;
  int fan_speed_;
  int program_;
  int season_;
  int out_reg_;

 protected:
  int state_{0};
  uint32_t last_send_{0};
  bool waiting_{false};
  bool waiting_for_write_ack_{false};

  std::deque<WriteableData>writequeue_;

  sensor::Sensor *air_temperature_sensor_{nullptr};
  sensor::Sensor *water_temperature_sensor_{nullptr};
  sensor::Sensor *fan_speed_sensor_{nullptr};
  sensor::Sensor *program_sensor_{nullptr};
  sensor::Sensor *season_sensor_{nullptr};
  sensor::Sensor *setpoint_sensor_{nullptr};
  sensor::Sensor *out_reg_sensor_{nullptr};
  binary_sensor::BinarySensor *boiler_relay_sensor_{nullptr};
  binary_sensor::BinarySensor *chiller_relay_sensor_{nullptr};

  CallbackManager<void(float)> target_temp_callback_;
  CallbackManager<void(float)> current_temp_callback_;
  CallbackManager<void(int)> fan_mode_callback_;
  CallbackManager<void(int)> climate_mode_callback_;
  CallbackManager<void(int)> climate_action_callback_;
  CallbackManager<void(bool)> key_lock_callback_;
};

}  // namespace innova
}  // namespace esphome
