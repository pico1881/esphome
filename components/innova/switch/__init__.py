import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID
from .. import Innova, CONF_INNOVA_ID

innova_ns = cg.esphome_ns.namespace("innova")
InnovaSwitch = innova_ns.class_("InnovaSwitch", switch.Switch, cg.Component)

DEPENDENCIES = ['innova']

CONFIG_SCHEMA = switch.SWITCH_SCHEMA.extend({
            cv.GenerateID(): cv.declare_id(InnovaSwitch),
            cv.GenerateID(CONF_INNOVA_ID): cv.use_id(Innova),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    innova = await cg.get_variable(config[CONF_INNOVA_ID])
    var = cg.new_Pvariable(config[CONF_ID], innova)
    await cg.register_component(var, config)
    await switch.register_switch(var, config)
