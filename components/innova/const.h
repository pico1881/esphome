#pragma once

namespace esphome {
namespace innova {

static const uint16_t INNOVA_AIR_TEMPERATURE = 0x00;
static const uint16_t INNOVA_WATER_TEMPERATURE = 0x01;
static const uint16_t INNOVA_REAL_SETPOINT = 0x08;
static const uint16_t INNOVA_FAN_SPEED = 0x0F;
static const uint16_t INNOVA_OUTPUT_RELAY = 0x09;
static const uint16_t INNOVA_PROGRAM = 0xC9;
static const uint16_t INNOVA_SEASON = 0xE9;
static const uint16_t INNOVA_SETPOINT = 0xE7;
static const uint16_t INNOVA_STATUS_FLAG = 0x68;
static const uint16_t INNOVA_ALARM_FLAG = 0x69;
static const uint8_t CMD_READ_REG = 0x03;
static const uint8_t CMD_WRITE_REG = 0x06;


}  // namespace innova
}  // namespace esphome
