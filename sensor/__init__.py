import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor

from .. import espnow_ns

CONF_ESPNOW_PEER = "peer"

ESPNowSensor = espnow_ns.class_("ESPNowSensor", sensor.Sensor, cg.Component)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        ESPNowSensor,
    )
    .extend(
        {
            cv.GenerateID(CONF_ESPNOW_PEER): cv.use_id(espnow_ns.ESPNowPeer),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    peer = await cg.get_variable(config[CONF_ESPNOW_PEER])
    cg.add(var.set_peer(peer))