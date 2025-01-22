import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
            CONF_ID, 
            CONF_DEVICE_CLASS, 
            DEVICE_CLASS_EMPTY,
            ENTITY_CATEGORY_NONE
            )
from .. import Innova, CONF_INNOVA_ID

DEPENDENCIES = ['innova']

CONF_BOILER_RELAY = "boiler_relay"
CONF_CHILLER_RELAY = "chiller_relay"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_INNOVA_ID): cv.use_id(Innova),
    cv.Optional(CONF_BOILER_RELAY): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_EMPTY,
        entity_category=ENTITY_CATEGORY_NONE,
    ),
    cv.Optional(CONF_CHILLER_RELAY): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_EMPTY,
        entity_category=ENTITY_CATEGORY_NONE,
    ),
}

async def to_code(config):
    var = await cg.get_variable(config[CONF_INNOVA_ID])

    if CONF_BOILER_RELAY in config:
        conf = config[CONF_BOILER_RELAY]
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_boiler_relay_sensor(sens))
    if CONF_CHILLER_RELAY in config:
        conf = config[CONF_CHILLER_RELAY]        
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(var.set_chiller_relay_sensor(sens))

