import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, sensor

from esphome.const import (
    CONF_ID, 
    CONF_TEMPERATURE, 
    CONF_HUMIDITY
)

innova_ns = cg.esphome_ns.namespace("innova")
Innova = innova_ns.class_(
    "Innova", climate.Climate, cg.PollingComponent
)


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Innova),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(),
            cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(),
        }
    )
    .extend(cv.polling_component_schema('60s'))
    .extend(climate.CLIMATE_SCHEMA)
)

async def to_code(config):
    # Create an instance of the CustomClimate component
    var = cg.new_Pvariable(config[CONF_ID])
    
    if CONF_TEMPERATURE in config:
        conf = config[CONF_TEMPERATURE]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_temperature_sensor(sens))

    if CONF_HUMIDITY in config:
        conf = config[CONF_HUMIDITY]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_humidity_sensor(sens))
        

    # Set up the custom climate component
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

