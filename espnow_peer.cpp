#include <cstring>
#include <string>

#include "espnow_client.h"
#include "espnow_peer.h"
#include "espnow_proto.h"

#include "esphome/core/log.h"

using namespace esphome;

namespace espnow
{
    Message::Message(uint8_t* data, size_t size)
    {
        this->size = size;
        this->data = new uint8_t[size];
        std::memcpy(this->data, data, size);
    }

    void ESPNowPeer::setup()
    {
        global_espnow_client->regsiter_peer(this);

#ifdef USE_BINARY_SENSOR
        for (binary_sensor::BinarySensor *obj : this->binary_sensors_)
            obj->add_on_state_callback([this, obj](bool state) { this->on_binary_sensor_update(obj, state); });
#endif

#ifdef USE_FAN
        for (fan::Fan *obj : this->fans_watched_)
            obj->add_on_state_callback([this, obj]() { this->on_fan_update(obj); });
#endif

#ifdef USE_LIGHT
        for (light::LightState *obj : this->lights_watched_)
            obj->add_new_remote_values_callback([this, obj]() { this->on_light_update(obj); });
#endif

#ifdef USE_COVER
#endif

#ifdef USE_SENSOR
        for (sensor::Sensor *obj : this->sensors_)
            obj->add_on_state_callback([this, obj](float state) { this->on_sensor_update(obj, state); });
#endif

#ifdef USE_TEXT_SENSOR
        for (text_sensor::TextSensor *obj : this->text_sensors_)
            obj->add_on_state_callback([this, obj](const std::string &state) { this->on_text_sensor_update(obj, state); });
#endif

#ifdef USE_SWITCH
        for (switch_::Switch *obj : this->switches_watched_)
            obj->add_on_state_callback([this, obj](bool state) { this->on_switch_update(obj, state); });
#endif

#ifdef USE_BUTTON
        for (button::Button *obj : this->buttons_)
            obj->add_on_press_callback([this, obj]() { this->on_button_update(obj); });
#endif

#ifdef USE_CLIMATE
        for (climate::Climate *obj : this->climates_watched_)
            obj->add_on_state_callback([this, obj]() { this->on_climate_update(obj); });
#endif

#ifdef USE_NUMBER
#endif

#ifdef USE_SELECT
#endif

#ifdef USE_LOCK
#endif
        this->set_interval(500, [this]() { this->periodic_send(); });
    }

    void ESPNowPeer::periodic_send()
    {
        if (this->message_buffer_.size() > 0)
        {
            if (this->message_buffer_.front().retries > 0)
            {
                ESP_LOGD(TAG, "Trying to send... (%d/10)", this->message_buffer_.front().retries);
                this->message_buffer_.front().retries--;
                global_espnow_client->send(this->address_, this->message_buffer_.front().data, this->message_buffer_.front().size);
            }
            else
            {
                uint8_t *data = this->message_buffer_.front().data;
                this->message_buffer_.pop();
                delete[] data;
            }
        }
    }

    void ESPNowPeer::set_address(uint64_t address)
    {
        for (uint8_t i = 0; i < 6; i++)
            this->address_[5 - i] = (address >> 8 * i) & 0xFF;
    }

    void ESPNowPeer::recv_handler(const uint8_t *data, int len)
    {
        ESP_LOGD(TAG, "Received message");
        uint32_t type = *(uint32_t *)data;

        switch (type)
        {

#ifdef USE_BINARY_SENSOR
        case enums::MESSAGE_TYPE_BINARY_SENSOR:
            this->set_binary_sensor_state((BinarySensorStateMessage *)data);
            break;
#endif

#ifdef USE_FAN
        case enums::MESSAGE_TYPE_FAN:
            this->set_fan_state((FanStateMessage *)data);
            break;
#endif

#ifdef USE_LIGHT
        case enums::MESSAGE_TYPE_LIGHT:
            this->set_light_state((LightStateMessage *)data);
            break;
#endif

#ifdef USE_COVER
#endif

#ifdef USE_SENSOR
        case enums::MESSAGE_TYPE_SENSOR:
            this->set_sensor_state((SensorStateMessage *)data);
            break;
#endif

#ifdef USE_TEXT_SENSOR
        case enums::MESSAGE_TYPE_TEXT_SENSOR:
            this->set_text_sensor_state((TextSensorStateMessage *)data);
            break;
#endif

#ifdef USE_SWITCH
    case enums::MESSAGE_TYPE_SWITCH:
        this->set_switch_state((SwitchStateMessage *)data);
        break;
#endif

#ifdef USE_BUTTON
        case enums::MESSAGE_TYPE_BUTTON:
            this->set_button_state((ButtonStateMessage *)data);
            break;
#endif

#ifdef USE_CLIMATE
        case enums::MESSAGE_TYPE_CLIMATE:
            this->set_climate_state((ClimateStateMessage *)data);
            break;
#endif

#ifdef USE_NUMBER
#endif

#ifdef USE_SELECT
#endif

#ifdef USE_LOCK
#endif

        case enums::MESSAGE_TYPE_UNKNOWN:
        default:
            ESP_LOGD(TAG, "Received message of unknown type");
        }
    }

    void ESPNowPeer::send_handler(bool status)
    {
        if (status)
        {
            uint8_t *data = this->message_buffer_.front().data;
            this->message_buffer_.pop();
            delete[] data;
        }
    }

#ifdef USE_BINARY_SENSOR
    void ESPNowPeer::register_binary_sensor(binary_sensor::BinarySensor *obj)
    {
        this->binary_sensors_.push_back(obj);
    }

    void ESPNowPeer::register_espnow_binary_sensor(binary_sensor::BinarySensor *obj)
    {
        this->espnow_binary_sensors_.push_back(obj);
    } 

    void ESPNowPeer::on_binary_sensor_update(binary_sensor::BinarySensor *obj, bool state)
    {
        ESP_LOGD(TAG, "Fan Component %s has updated", obj->get_object_id().c_str());

        BinarySensorStateMessage msg;
        
        msg.key = obj->get_object_id_hash();
        msg.state = state;

        Message message((uint8_t *)&msg, sizeof(msg));
        this->message_buffer_.push(message);
    }

    void ESPNowPeer::set_binary_sensor_state(BinarySensorStateMessage *msg)
    {
        binary_sensor::BinarySensor *obj = get_espnow_binary_sensor_by_key(msg->key);
        if (obj == nullptr)
        {
            ESP_LOGE(TAG, "Binary Sensor not found (%d)", msg->key);
            return;
        }

        ESP_LOGD(TAG, "Applying state for Binary Sensor %s", obj->get_object_id().c_str());

        obj->publish_state(msg->state);
    }

    binary_sensor::BinarySensor* ESPNowPeer::get_espnow_binary_sensor_by_key(uint32_t key)
    {
        for (binary_sensor::BinarySensor *obj : this->espnow_binary_sensors_)
            if (obj->get_object_id_hash() == key)
                return obj;
        return nullptr;
    }

#endif
#ifdef USE_FAN
    void ESPNowPeer::register_fan(fan::Fan *obj, bool watch)
    {
        if (watch && !obj->is_internal())
            this->fans_watched_.push_back(obj);
        else
            this->fans_.push_back(obj);
    }

    void ESPNowPeer::on_fan_update(fan::Fan *obj)
    {
        ESP_LOGD(TAG, "Fan Component %s has updated", obj->get_object_id().c_str());

        FanStateMessage msg;

        auto traits = obj->get_traits();

        msg.key = obj->get_object_id_hash();
        msg.state = obj->state;
        if (traits.supports_oscillation())
        {
            msg.has_oscillating = true;
            msg.oscillating = obj->oscillating;
        }
        if (traits.supports_speed())
        {
            msg.has_speed = true;
            msg.speed = obj->speed;
        }
        if (traits.supports_direction())
        {
            msg.has_direction = true;
            msg.direction = static_cast<enums::FanDirection>(obj->direction);
        }

        Message message((uint8_t *)&msg, sizeof(msg));
        this->message_buffer_.push(message);
    }

    void ESPNowPeer::set_fan_state(FanStateMessage *msg)
    {
        fan::Fan *obj = this->get_fan_by_key(msg->key);
        if (obj == nullptr)
        {
            ESP_LOGE(TAG, "Fan not found (%d)", msg->key);
            return;
        }

        ESP_LOGD(TAG, "Applying fan state for %s", obj->get_object_id().c_str());

        auto call = obj->make_call();

        call.set_state(msg->state);
        if (msg->has_oscillating)
            call.set_oscillating(msg->oscillating);
        if (msg->has_speed)
            call.set_speed(msg->speed);
        if (msg->has_direction)
            call.set_direction(static_cast<fan::FanDirection>(msg->direction));

        call.perform();
    }

    fan::Fan *ESPNowPeer::get_fan_by_key(uint32_t key)
    {
        for (fan::Fan *obj : this->fans_)
            if (obj->get_object_id_hash() == key)
                return obj;
        return nullptr;
    }
#endif

#ifdef USE_LIGHT
    void ESPNowPeer::register_light(light::LightState *obj, bool watch)
    {
        if (watch && !obj->is_internal())
            this->lights_watched_.push_back(obj);
        else
            this->lights_.push_back(obj);
    }

    void ESPNowPeer::on_light_update(light::LightState *obj)
    {
        ESP_LOGD(TAG, "Light Component %s has updated", obj->get_object_id().c_str());

        LightStateMessage msg;

        auto values = obj->remote_values;
        auto color_mode = values.get_color_mode();

        msg.key = obj->get_object_id_hash();
        msg.state = values.is_on();
        msg.brightness = values.get_brightness();
        msg.color_mode = static_cast<enums::ColorMode>(color_mode);
        msg.color_brightness = values.get_color_brightness();
        msg.red = values.get_red();
        msg.green = values.get_green();
        msg.blue = values.get_blue();
        msg.white = values.get_white();
        msg.color_temperature = values.get_color_temperature();
        msg.cold_white = values.get_cold_white();
        msg.warm_white = values.get_warm_white();
        if (obj->supports_effects())
            msg.effect = obj->get_effect_name();

        Message message((uint8_t *)&msg, sizeof(msg));
        this->message_buffer_.push(message);
    }

    void ESPNowPeer::set_light_state(LightStateMessage *msg)
    {
        light::LightState *obj = this->get_light_by_key(msg->key);
        if (obj == nullptr)
        {
            ESP_LOGE(TAG, "Light not found (%d)", msg->key);
            return;
        }

        ESP_LOGD(TAG, "Applying light state for %s", obj->get_name().c_str());

        auto call = obj->make_call();
        call.set_state(msg->state);
        call.set_brightness_if_supported(msg->brightness);
        call.set_color_mode_if_supported(static_cast<light::ColorMode>(msg->color_mode));
        call.set_color_brightness_if_supported(msg->color_brightness);
        call.set_red_if_supported(msg->red);
        call.set_green_if_supported(msg->green);
        call.set_blue_if_supported(msg->blue);
        call.set_white_if_supported(msg->white);
        call.set_color_temperature_if_supported(msg->color_temperature);
        call.set_cold_white_if_supported(msg->cold_white);
        call.set_warm_white_if_supported(msg->warm_white);
        if (obj->supports_effects())
            call.set_effect(msg->effect);

        call.perform();
    }

    light::LightState *ESPNowPeer::get_light_by_key(uint32_t key)
    {
        for (light::LightState *obj : this->lights_)
            if (obj->get_object_id_hash() == key)
                return obj;
        return nullptr;
    }
#endif

#ifdef USE_SENSOR
    void ESPNowPeer::register_sensor(sensor::Sensor *obj)
    {
        this->sensors_.push_back(obj);
    }

    void ESPNowPeer::register_espnow_sensor(sensor::Sensor *obj)
    {
        this->espnow_sensors_.push_back(obj);
    }
    void ESPNowPeer::on_sensor_update(sensor::Sensor *obj, float state)
    {
        ESP_LOGD(TAG, "Sensor %s has updated", obj->get_object_id().c_str());

        SensorStateMessage msg;

        msg.key = obj->get_object_id_hash();
        msg.state = state;

        Message message((uint8_t *)&msg, sizeof(msg));
        this->message_buffer_.push(message);
    }

    void ESPNowPeer::set_sensor_state(SensorStateMessage *msg)
    {
        sensor::Sensor *obj = this->get_espnow_sensor_by_key(msg->key);
        if (obj == nullptr)
        {
            ESP_LOGE(TAG, "Sensor not found (%d)", msg->key);
            return;
        }

        obj->publish_state(msg->state);
    }

    sensor::Sensor *ESPNowPeer::get_espnow_sensor_by_key(uint32_t key)
    {
        for (sensor::Sensor *obj : this->espnow_sensors_)
            if (obj->get_object_id_hash() == key)
                return obj;
        return nullptr;
    }
#endif

#ifdef USE_TEXT_SENSOR
    void ESPNowPeer::register_text_sensor(text_sensor::TextSensor *obj)
    {
        this->text_sensors_.push_back(obj);
    }

    void ESPNowPeer::register_espnow_text_sensor(text_sensor::TextSensor *obj) //TODO change type
    {
        this->espnow_text_sensors_.push_back(obj);
    }

    void ESPNowPeer::on_text_sensor_update(text_sensor::TextSensor *obj, const std::string &state)
    {
        ESP_LOGD(TAG, "Text Sensor %s has updated", obj->get_object_id().c_str());

        TextSensorStateMessage msg;

        msg.key = obj->get_object_id_hash();
        msg.state = state;

        Message message((uint8_t *)&msg, sizeof(msg));

        this->message_buffer_.push(message);
    }

    void ESPNowPeer::set_text_sensor_state(TextSensorStateMessage *msg)
    {
        text_sensor::TextSensor *obj = this->get_espnow_text_sensor_by_key(msg->key);   //TODO change type
        if (obj == nullptr)
        {
            ESP_LOGE(TAG, "Text Sensor not found (%d)" msg->key);
            return;
        }

        obj->publish_state(msg->state);
    }

    text_sensor::TextSensor* ESPNowPeer::get_espnow_text_sensor_by_key(uint32_t key) //TODO change type
    {
        for (text_sensor::TextSensor *obj : this->espnow_text_sensors_)
            if (obj->get_object_id_hash() == key)
                return obj;
        return nullptr;
    }
#endif

#ifdef USE_SWITCH
    void ESPNowPeer::register_switch(switch_::Switch *obj, bool watch)
    {
        if (watch && !obj->is_internal())
            this->switches_watched_.push_back(obj);
        else
            this->switches_.push_back(obj);
    }

    void ESPNowPeer::on_switch_update(switch_::Switch *obj, bool state)
    {
        ESP_LOGD(TAG, "Switch %s has updated", obj->get_object_id().c_str());

        SwitchStateMessage msg;

        msg.key = obj->get_object_id_hash();
        msg.state = state;

        Message message((uint8_t *)&msg, sizeof(msg));
        this->message_buffer_.push(message);
    }

    void ESPNowPeer::set_switch_state(SwitchStateMessage *msg)
    {
        switch_::Switch *obj = this->get_switch_by_key(msg->key);
        if (obj == nullptr)
        {
            ESP_LOGE(TAG, "Switch not found (%d)", msg->key);
            return;
        }

        obj->publish_state(msg->state);   
    }

    switch_::Switch* ESPNowPeer::get_switch_by_key(uint32_t key)
    {
        for (switch_::Switch *obj : this->switches_)
            if (obj->get_object_id_hash() == key)
                return obj;
        return nullptr;
    }

#endif

#ifdef USE_BUTTON
    void ESPNowPeer::register_button(button::Button *obj, bool watch)
    {
        if (watch && !obj->is_internal())
            this->buttons_watched_.push_back(obj);
        else
            this->buttons_.push_back(obj);
    }

    void ESPNowPeer::on_button_update(button::Button *obj)
    {
        ESP_LOGD(TAG, "Button %s has updated", obj->get_object_id().c_str());

        ButtonStateMessage msg;

        msg.key = obj->get_object_id_hash();

        Message message((uint8_t *)&msg, sizeof(msg));
        this->message_buffer_.push(message);
    }

    void ESPNowPeer::set_button_state(ButtonStateMessage *msg)
    {
        button::Button *obj = this->get_button_by_key(msg->key);
        if (obj == nullptr)
        {
            ESP_LOGE(TAG, "Button not found (%d)" msg->key);
            return;
        }

        obj->press();
    }

    button::Button* ESPNowPeer::get_button_by_key(uint32_t key)
    {
        for (button::Button *obj : this->buttons_)
            if (obj->get_object_id_hash() == key)
                return obj;
        return nullptr;
    }

#endif

#ifdef USE_CLIMATE
    void ESPNowPeer::register_climate(climate::Climate *obj, bool watch)
    {
        if (watch && !obj->is_internal())
            this->climates_watched_.push_back(obj);
        else
            this->climates_.push_back(obj);
    }

    void ESPNowPeer::on_climate_update(climate::Climate *obj)
    {
        ESP_LOGD(TAG, "Climate %s has updated", obj->get_object_id().c_str());

        auto traits = obj->get_traits();

        ClimateStateMessage msg;

        msg.key = obj->get_object_id_hash();
        msg.mode = static_cast<enums::ClimateMode>(obj->mode);
        if (traits.get_supports_two_point_target_temperature())
        {
            msg.has_two_point_target_temperature = true;
            msg.target_temperature_high = obj->target_temperature_high;
            msg.target_temperature_low = obj->target_temperature_low;
        }
        else
        {
            msg.has_target_temperature = true;
            msg.target_temperature = obj->target_temperature;
        }
        if (traits.get_supports_fan_modes() && obj->fan_mode.has_value())
        {
            msg.has_fan_mode = true;
            msg.fan_mode = static_cast<enums::ClimateFanMode>(obj->fan_mode.value());
        }
        if (!traits.get_supported_custom_fan_modes().empty() && obj->custom_fan_mode.has_value())
        {
            msg.has_custom_fan_mode = true;
            msg.custom_fan_mode = obj->custom_fan_mode.value();
        }
        if (traits.get_supports_presets() && obj->preset.has_value())
        {
            msg.has_preset = true;
            msg.preset = static_cast<enums::ClimatePreset>(obj->preset.value());
        }
        if (!traits.get_supported_custom_presets().empty() && obj->custom_preset.has_value())
        {
            msg.has_custom_preset = true;
            msg.custom_preset = obj->custom_preset.value();
        }
        if (traits.get_supports_swing_modes())
        {
            msg.has_swing_mode = true;
            msg.swing_mode = static_cast<enums::ClimateSwingMode>(obj->swing_mode);
        }

        Message message((uint8_t *)&msg, sizeof(msg));
        this->message_buffer_.push(message);
    }

    void ESPNowPeer::set_climate_state(ClimateStateMessage *msg)
    {
        climate::Climate *obj = this->get_climate_by_key(msg->key);
        if (obj == nullptr)
        {
            ESP_LOGE(TAG, "Climate not found (%d)", msg->key);
            return;
        }

        ESP_LOGD(TAG, "Applying Climate state for %s", obj->get_name().c_str());

        auto call = obj->make_call();

        call.set_mode(static_cast<climate::ClimateMode>(msg->mode));
        if (msg->has_target_temperature)
        {
            call.set_target_temperature(msg->target_temperature);
        }
        else if (msg->has_two_point_target_temperature)
        {
            call.set_target_temperature_high(msg->target_temperature_high);
            call.set_target_temperature_low(msg->target_temperature_low);
        }
        if (msg->has_fan_mode)
            call.set_fan_mode(static_cast<climate::ClimateFanMode>(msg->fan_mode));
        if (msg->has_custom_fan_mode)
            call.set_fan_mode(msg->custom_fan_mode);
        if (msg->has_preset)
            call.set_preset(static_cast<climate::ClimatePreset>(msg->preset));
        if (msg->has_custom_preset)
            call.set_preset(msg->custom_preset);
        if (msg->has_swing_mode)
            call.set_swing_mode(static_cast<climate::ClimateSwingMode>(msg->swing_mode));
        call.perform();
    }

    climate::Climate *ESPNowPeer::get_climate_by_key(uint32_t key)
    {
        for (climate::Climate *obj : this->climates_)
            if (obj->get_object_id_hash() == key)
                return obj;
        return nullptr;
    }
#endif
}
