#pragma once

#include "../espnow_peer.h"

#ifdef USE_BINARY_SENSOR

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

using namespace esphome;

namespace espnow
{
    class ESPNowBinarySensor : public binary_sensor::BinarySensor, public Component
    {
    public:
        void setup() override { this->peer_->register_espnow_binary_sensor(this); }
        void set_peer(ESPNowPeer *peer) { this->peer_ = peer; }

    protected:
        ESPNowPeer *peer_;
    };
} // namespace espnow

#endif