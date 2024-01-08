import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light, sensor
from esphome.const import (
    CONF_MAC_ADDRESS,
    CONF_CHANNEL,
    CONF_ID,
    CONF_LIGHT,
    CONF_SENSOR,
)
from esphome.components.wifi import validate_channel
from .types import (
    espnow_ns,
    ESPNowClient,
    ESPNowPeer,
)
from .const import (
    CONF_ESPNOW,
    CONF_PEERS,
    CONF_WATCH,
)

LIGHT_SCHEMA = (
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(light.LightState),
            cv.Optional(CONF_WATCH, default=False): cv.boolean,
        }
    )
)

SENSOR_SCHEMA = (
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(sensor.Sensor),
        }
    )
)

PEER_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ESPNowPeer),
            cv.Required(CONF_MAC_ADDRESS): cv.mac_address,
            cv.Optional(CONF_LIGHT): cv.ensure_list(LIGHT_SCHEMA),
            cv.Optional(CONF_SENSOR): cv.ensure_list(SENSOR_SCHEMA),
        }
    )
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ESPNowClient),
            cv.Required(CONF_CHANNEL): validate_channel,
            cv.Required(CONF_PEERS): cv.ensure_list(PEER_SCHEMA),
        }
    ),
    cv.only_on(["esp32", "esp8266"])
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_channel(config[CONF_CHANNEL]))

    for peer_config in config.get(CONF_PEERS, []):
        peer = cg.new_Pvariable(peer_config[CONF_ID])
        await cg.register_component(peer, peer_config)
        cg.add(peer.set_address(peer_config[CONF_MAC_ADDRESS].as_hex))

        for light_config in peer_config.get(CONF_LIGHT, []):
            light = await cg.get_variable(light_config[CONF_ID])
            cg.add(peer.register_light(light, light_config[CONF_WATCH]))

        for sensor_config in peer_config.get(CONF_SENSOR, []):
            sensor = await cg.get_variable(sensor_config[CONF_ID])
            cg.add(peer.register_sensor(sensor))