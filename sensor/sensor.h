#pragma once

#include "../espnow_peer.h"

#ifdef USE_SENSOR

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

using namespace esphome;

namespace espnow
{
    class ESPNowSensor : public sensor::Sensor, public Component
    {
    public:
        void setup() override { this->peer_->register_espnow_sensor(this); }
        void set_peer(ESPNowPeer *peer) { this->peer_ = peer; }

    protected:
        ESPNowPeer *peer_;
    };
} // namespace espnow

#endif