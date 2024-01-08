import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor

from .. import espnow_ns

CONF_ESPNOW_PEER = "peer"

ESPNowBinarySensor = espnow_ns.class_("ESPNowBinarySensor", binary_sensor.BinarySensor, cg.Component)

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema(
        ESPNowBinarySensor,
    )
    .extend(
        {
            cv.GenerateID(CONF_ESPNOW_PEER): cv.use_id(espnow_ns.ESPNowPeer),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    var = await binary_sensor.new_binary_sensor(config)
    await cg.register_component(var, config)

    peer = await cg.get_variable(config[CONF_ESPNOW_PEER])
    cg.add(var.set_peer(peer))