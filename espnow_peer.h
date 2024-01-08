#pragma once

#include <queue>

#include "espnow_proto.h"

#include "esphome/core/defines.h"
#include "esphome/core/component.h"

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

#ifdef USE_FAN
#include "esphome/components/fan/fan_state.h"
#endif

#ifdef USE_LIGHT
#include "esphome/components/light/light_state.h"
#endif

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif 

#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif

#ifdef USE_CLIMATE
#include "esphome/components/climate/climate.h"
#endif

using namespace esphome;
namespace espnow
{
    class Message
    {
    public:
        Message(uint8_t* data, size_t size);
        size_t size;
        uint8_t* data;
        uint8_t retries = 10;
    };

    class ESPNowPeer : public Component
    {
    public:
        void setup() override;
        float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }
        
        void periodic_send();

        void set_address(uint64_t address);
        uint8_t *get_address() { return this->address_; }

        void recv_handler(const uint8_t *data, int len);
        void send_handler(bool status);

#ifdef USE_BINARY_SENSOR
        void register_binary_sensor(binary_sensor::BinarySensor *obj);
        void register_espnow_binary_sensor(binary_sensor::BinarySensor *obj);
        void on_binary_sensor_update(binary_sensor::BinarySensor *obj, bool state);
#endif

#ifdef USE_FAN
        void register_fan(fan::Fan *obj, bool watch = false);
        void on_fan_update(fan::Fan *obj);
#endif

#ifdef USE_LIGHT
        void register_light(light::LightState *obj, bool watch = false);
        void on_light_update(light::LightState *obj);
#endif

#ifdef USE_SENSOR
        void register_sensor(sensor::Sensor *obj);
        void register_espnow_sensor(sensor::Sensor *obj);
        void on_sensor_update(sensor::Sensor *obj, float state);
#endif

#ifdef USE_TEXT_SENSOR
        void register_text_sensor(text_sensor::TextSensor *obj);
        void register_espnow_text_sensor(text_sensor::TextSensor *obj);
        void on_text_sensor_update(text_sensor::TextSensor *obj, const std::string &state);
#endif

#ifdef USE_SWITCH
        void register_switch(switch_::Switch *obj, bool watch = false);
        void on_switch_update(switch_::Switch *obj, bool state);
#endif

#ifdef USE_BUTTON
        void register_button(button::Button *obj, bool watch = false);
        void on_button_update(button::Button *obj);
#endif

#ifdef USE_CLIMATE
        void register_climate(climate::Climate *obj, bool watch = false);
        void on_climate_update(climate::Climate *obj);
#endif

    protected:
        uint8_t address_[6];
        std::queue<Message> message_buffer_;

#ifdef USE_BINARY_SENSOR
        void set_binary_sensor_state(BinarySensorStateMessage *msg);
        binary_sensor::BinarySensor* get_espnow_binary_sensor_by_key(uint32_t key);
        std::vector<binary_sensor::BinarySensor *> binary_sensors_;
        std::vector<binary_sensor::BinarySensor *> espnow_binary_sensors_;
#endif

#ifdef USE_FAN
        void set_fan_state(FanStateMessage *msg);
        fan::Fan* get_fan_by_key(uint32_t key);
        std::vector<fan::Fan *> fans_;
        std::vector<fan::Fan *> fans_watched_;
#endif

#ifdef USE_LIGHT
        void set_light_state(LightStateMessage *msg);
        light::LightState* get_light_by_key(uint32_t key);
        std::vector<light::LightState *> lights_;
        std::vector<light::LightState *> lights_watched_;
#endif

#ifdef USE_SENSOR
        void set_sensor_state(SensorStateMessage *msg);
        sensor::Sensor* get_espnow_sensor_by_key(uint32_t key);
        std::vector<sensor::Sensor *> sensors_;
        std::vector<sensor::Sensor *> espnow_sensors_;
#endif

#ifdef USE_TEXT_SENSOR
        void set_text_sensor_state(TextSensorStateMessage *msg);
        text_sensor::TextSensor* get_espnow_text_sensor_by_key(uint32_t key);
        std::vector<text_sensor::TextSensor *> text_sensors_;
        std::vector<text_sensor::TextSensor *> espnow_text_sensors_;
#endif

#ifdef USE_SWITCH
        void set_switch_state(SwitchStateMessage *msg);
        switch_::Switch* get_switch_by_key(uint32_t key);
        std::vector<switch_::Switch *> switches_;
        std::vector<switch_::Switch *> switches_watched_;
#endif

#ifdef USE_BUTTON
        void set_button_state(ButtonStateMessage *msg);
        button::Button* get_button_by_key(uint32_t key);
        std::vector<button::Button *> buttons_;
        std::vector<button::Button *> buttons_watched_;
#endif

#ifdef USE_CLIMATE
        void set_climate_state(ClimateStateMessage *msg);
        climate::Climate* get_climate_by_key(uint32_t key);
        std::vector<climate::Climate *> climates_;
        std::vector<climate::Climate *> climates_watched_;
#endif

    };
}