#include "espnow_client.h"

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#ifdef USE_ESP8266
#include <espnow.h>
#include <user_interface.h>
#endif

#ifdef USE_ESP32
#include <esp_now.h>
#include <WiFi.h>
#endif

namespace espnow
{
    ESPNowClient *global_espnow_client = nullptr;
    
    ESPNowClient::ESPNowClient()
    {
        global_espnow_client = this;
    }

    void ESPNowClient::setup()
    {
#ifdef USE_ESP8266
        wifi_set_opmode_current(1);
#endif

#ifdef USE_ESP32
        WiFi.mode(WIFI_STA);
#endif
        if(esp_now_init() != 0)
        {
            ESP_LOGE(TAG, "Init Failed!");
            this->mark_failed();
            for (ESPNowPeer *peer : this->peers_)
            {
                peer->mark_failed();
            }
            return;
        }
        ESP_LOGCONFIG(TAG, "Init OK!");

#ifdef USE_ESP8266
        esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
#endif
        esp_now_register_recv_cb(espnow::recv_handler);
        esp_now_register_send_cb(espnow::send_handler);
    }

    void ESPNowClient::regsiter_peer(ESPNowPeer *peer)
    {
        this->peers_.push_back(peer);
        uint8_t *mac_addr = peer->get_address();
        
#ifdef USE_ESP8266
        int status = esp_now_add_peer(mac_addr, ESP_NOW_ROLE_COMBO, channel_, NULL, 0);
#endif

#ifdef USE_ESP32
        esp_now_peer_info_t peer_info = {};
        memcpy(peer_info.peer_addr, mac_addr, 6);
        peer_info.channel = this->channel_;
        peer_info.encrypt = false;

        if(esp_now_init() != 0)
        {
            ESP_LOGD(TAG, "esp_now_init failed!");
        }
        esp_err_t status = esp_now_add_peer(&peer_info);
#endif

        if(status != 0)
        {
            peer->mark_failed();
            ESP_LOGE(TAG, "Error while adding peer %d", status);
            return;
        }

        ESP_LOGD(TAG, "Added Peer: Address: %s", format_mac_addr(mac_addr).c_str());
    }

    void ESPNowClient::recv_handler(const uint8_t *mac_addr, const uint8_t *data, int len)
    {
        for(int i = 0; i < peers_.size(); i++)
        {
            if(memcmp(peers_[i]->get_address(), mac_addr, 6) == 0)
            {
                uint8_t* d = (uint8_t*) malloc(len);
                memcpy(d, data, len);
                peers_[i]->recv_handler(d, len);
            }
        }
    }

    void ESPNowClient::send_handler(const uint8_t *mac_addr, bool status)
    {
        for(int i = 0; i < peers_.size(); i++)
        {
            if(memcmp(peers_[i]->get_address(), mac_addr, 6) == 0)
            {
                peers_[i]->send_handler(status);
            }
        }
    }

    void ESPNowClient::send(uint8_t *mac_addr, uint8_t *data, int len)
    {
        esp_now_send(mac_addr, data, len);
    }

    std::string format_mac_addr(const uint8_t *mac)
    {
        return str_snprintf("%02X:%02X:%02X:%02X:%02X:%02X", 17,mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

#ifdef USE_ESP8266
    void recv_handler(uint8_t *mac_addr, uint8_t *data, uint8_t len)
    {
        ESP_LOGD(TAG, "received %d bytes from %s", len, format_mac_addr(mac_addr).c_str());

        global_espnow_client->recv_handler(mac_addr, data, len);  
    }

    void send_handler(uint8_t *mac_addr, uint8_t status)
    {
        ESP_LOGD(TAG, "Send to %s : %s", format_mac_addr(mac_addr).c_str(), (status == 0) ? "Ok" : "Failed");

        global_espnow_client->send_handler(mac_addr, status == 0);
    }
#endif

#ifdef USE_ESP32
    void recv_handler(const uint8_t *mac_addr, const uint8_t *data, int len)
    {
        ESP_LOGD(TAG, "received %d bytes from %s", len, format_mac_addr(mac_addr).c_str());

        global_espnow_client->recv_handler(mac_addr, data, len);  
    }

    void send_handler(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
        bool status_ = (status == ESP_NOW_SEND_SUCCESS);
        ESP_LOGD(TAG, "Send to %s : %s", format_mac_addr(mac_addr).c_str(), status_ ? "Ok" : "Failed");

        global_espnow_client->send_handler(mac_addr, status_);
    }
#endif

}