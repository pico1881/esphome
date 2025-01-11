#pragma once

namespace esphome {
namespace innova {

static const uint16_t INNOVA_AIR_TEMPERATURE = 0x0000;
static const uint16_t INNOVA_WATER_TEMPERATURE = 0x0002;
static const uint16_t INNOVA_REAL_SETPOINT = 0x0008;
static const uint16_t INNOVA_FAN_SPEED = 0x000F;
static const uint16_t INNOVA_OUTPUT_RELAY = 0x0009;
static const uint16_t INNOVA_PROGRAM = 0x00C9;
static const uint16_t INNOVA_SEASON = 0x00E9;
static const uint16_t INNOVA_SETPOINT = 0x00E7;
static const uint16_t INNOVA_STATUS_FLAG = 0x0068;
static const uint16_t INNOVA_ALARM_FLAG = 0x0069;
static const uint8_t CMD_READ_REG = 0x03;
static const uint8_t CMD_WRITE_REG = 0x06;


}  // namespace innova
}  // namespace esphome
