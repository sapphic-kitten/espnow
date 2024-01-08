import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor

from .. import espnow_ns

CONF_ESPNOW_PEER = "peer"

ESPNowTextSensor = espnow_ns.class_("ESPNowTextSensor", text_sensor.TextSensor, cg.Component)

CONFIG_SCHEMA = (
    text_sensor.text_sensor_schema(
        ESPNowTextSensor,
    )
    .extend(
        {
            cv.GenerateID(CONF_ESPNOW_PEER): cv.use_id(espnow_ns.ESPNowPeer),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    var = await text_sensor.new_text_sensor(config)
    await cg.register_component(var, config)

    peer = await cg.get_variable(config[CONF_ESPNOW_PEER])
    cg.add(var.set_peer(peer))