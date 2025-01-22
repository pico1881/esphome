import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, modbus
from esphome.const import (
    CONF_TEMPERATURE,
    CONF_ID,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_SPEED,
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_MEASUREMENT,
    UNIT_REVOLUTIONS_PER_MINUTE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
)

AUTO_LOAD = ["modbus"]

innova_ns = cg.esphome_ns.namespace("innova")
INNOVA = innova_ns.class_("INNOVA", cg.PollingComponent, modbus.ModbusDevice)

CONF_WATER_TEMPERATURE = "water_temperature"
CONF_AIR_TEMPERATURE = "air_temperature"
CONF_FAN_SPEED = "fan_speed"
CONF_PROGRAM = "program"
CONF_SEASON = "season"
CONF_SETPOINT = "setpoint"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(INNOVA),
            cv.Optional(CONF_AIR_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_WATER_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_FAN_SPEED): sensor.sensor_schema(
                unit_of_measurement=UNIT_REVOLUTIONS_PER_MINUTE,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_PROGRAM): sensor.sensor_schema(
                unit_of_measurement=UNIT_EMPTY,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_SEASON): sensor.sensor_schema(
                unit_of_measurement=UNIT_EMPTY,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_SETPOINT): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(modbus.modbus_device_schema(0x01))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await modbus.register_modbus_device(var, config)

    if CONF_AIR_TEMPERATURE in config:
        conf = config[CONF_AIR_TEMPERATURE]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_air_temperature_sensor(sens))
    if CONF_WATER_TEMPERATURE in config:
        conf = config[CONF_WATER_TEMPERATURE]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_water_temperature_sensor(sens))
    if CONF_FAN_SPEED in config:
        conf = config[CONF_FAN_SPEED]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_fan_speed_sensor(sens))
    if CONF_PROGRAM in config:
        conf = config[CONF_PROGRAM]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_program_sensor(sens))
    if CONF_SEASON in config:
        conf = config[CONF_SEASON]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_season_sensor(sens))
    if CONF_SETPOINT in config:
        conf = config[CONF_SETPOINT]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_setpoint_sensor(sens))
