import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, modbus
from esphome.const import CONF_ID, CONF_UNIT_OF_MEASUREMENT

UNITS = {
    "f": "f",
    "c": "c",
}

innova_ns = cg.esphome_ns.namespace("innova")
Innova = innova_ns.class_(
    "Innova", climate.Climate, ble_client.BLEClientNode, cg.PollingComponent
)

CONFIG_SCHEMA = (
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Innova),
            cv.Required(CONF_UNIT_OF_MEASUREMENT): cv.enum(UNITS),
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)
    cg.add(var.set_unit_of_measurement(config[CONF_UNIT_OF_MEASUREMENT]))
