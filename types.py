import esphome.codegen as cg

espnow_ns = cg.global_ns.namespace('espnow')
ESPNowClient = espnow_ns.class_('ESPNowClient', cg.Component)
ESPNowPeer = espnow_ns.class_('ESPNowPeer', cg.Component)
