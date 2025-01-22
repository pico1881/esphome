import esphome.codegen as cg
import esphome.config_validation as cv
from .. import Innova, CONF_INNOVA_ID
from esphome.components import climate, sensor

from esphome.const import CONF_ID

innova_ns = cg.esphome_ns.namespace("innova")
InnovaClimate = innova_ns.class_("InnovaClimate", climate.Climate, cg.Component)


CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
            cv.GenerateID(): cv.declare_id(InnovaClimate),
            cv.GenerateID(CONF_INNOVA_ID): cv.use_id(Innova),
        }).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    innova = await cg.get_variable(config[CONF_INNOVA_ID])
    var = cg.new_Pvariable(config[CONF_ID], innova)
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

