#pragma once

#include "../espnow_peer.h"

#ifdef USE_TEXT_SENSOR

#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"

using namespace esphome;

namespace espnow
{
    class ESPNowTextSensor : public text_sensor::TextSensor, public Component
    {
    public:
        void setup() override { this->peer_->register_espnow_text_sensor(this); }
        void set_peer(ESPNowPeer *peer) { this->peer_ = peer; }

    protected:
        ESPNowPeer *peer_;
    };
}

#endif