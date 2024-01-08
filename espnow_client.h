#pragma once

#include "espnow_peer.h"

#include "esphome/core/component.h"

#ifdef USE_ESP32
#include <esp_now.h>
#endif

using namespace esphome;

namespace espnow
{
    static const char *TAG = "espnow"; 

    class ESPNowPeer;

    class ESPNowClient : public Component
    {
    public:
        ESPNowClient();
        void setup() override;
        float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

        void set_channel(uint8_t channel) {channel_ = channel; }

        void regsiter_peer(ESPNowPeer *peer);

        void recv_handler(const uint8_t *mac_addr, const uint8_t *data, int len);
        void send_handler(const uint8_t *mac_addr, bool status);

        void send(uint8_t *mac_addr, uint8_t *data, int len); 

    protected:
        uint8_t channel_;

        std::vector<ESPNowPeer *> peers_;
    };

    std::string format_mac_addr(const uint8_t *mac);
    

#ifdef USE_ESP8266
    void recv_handler(uint8_t *mac_addr, uint8_t *data, uint8_t len);
    void send_handler(uint8_t *mac_addr, uint8_t status);
#endif


#ifdef USE_ESP32
    void recv_handler(const uint8_t *mac_addr, const uint8_t *data, int len);
    void send_handler(const uint8_t *mac_addr, esp_now_send_status_t status);
#endif

    extern ESPNowClient *global_espnow_client;
}