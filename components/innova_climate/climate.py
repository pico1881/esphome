import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate

from esphome.const import (
    CONF_ID, 
)

innova_ns = cg.esphome_ns.namespace("innova")
Innova = innova_ns.class_(
    "Innova", climate.Climate, cg.PollingComponent
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Innova),
}).extend(cv.polling_component_schema('60s')).extend(climate.CLIMATE_SCHEMA)


        
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)


