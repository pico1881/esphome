import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor

from esphome.const import (
    CONF_ID, 
    CONF_TEMPERATURE, 
    CONF_HUMIDITY
)

innova_ns = cg.esphome_ns.namespace("innova")

Innova = innova_ns.class_("Innova", climate.Climate, cg.PollingComponent)


#CONFIG_SCHEMA = cv.Schema({
#    cv.GenerateID(): cv.declare_id(InnovaClimate),
#    cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(),
#    cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(),
#}).extend(cv.polling_component_schema('60s')).extend(climate.CLIMATE_SCHEMA)


CONFIG_SCHEMA = (
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Innova),
        }
    )
    .extend(cv.polling_component_schema("60s"))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    # Set up temperature sensor if defined
 #   if CONF_TEMPERATURE in config:
 #       sens = await sensor.new_sensor(config[CONF_TEMPERATURE])
 #       cg.add(var.set_temperature_sensor(sens))

    # Set up humidity sensor if defined
#    if CONF_HUMIDITY in config:
#        sens = await sensor.new_sensor(config[CONF_HUMIDITY])
 #       cg.add(var.set_humidity_sensor(sens))
