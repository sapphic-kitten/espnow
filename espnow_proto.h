#pragma once

#include <stdint.h>
#include <string>

namespace espnow
{
    namespace enums
    {
        enum MessageType : uint32_t
        {
            MESSAGE_TYPE_BINARY_SENSOR,
            MESSAGE_TYPE_FAN,
            MESSAGE_TYPE_LIGHT,
            MESSGAE_TYPE_COVER,
            MESSAGE_TYPE_SENSOR,
            MESSAGE_TYPE_TEXT_SENSOR,
            MESSAGE_TYPE_SWITCH,
            MESSAGE_TYPE_BUTTON,
            MESSAGE_TYPE_CLIMATE,
            MESSAGE_TYPE_NUMBER,
            MESSAGE_TYPE_SELECT,
            MESSAGE_TYPE_LOCK,
            MESSAGE_TYPE_UNKNOWN,
        };

        enum FanDirection : uint32_t
        {
            FAN_DIRECTION_FORWARD = 0,
            FAN_DIRECTION_REVERSE = 1,
        };

        enum ColorMode : uint32_t
        {
            COLOR_MODE_UNKNOWN = 0,
            COLOR_MODE_ON_OFF = 1,
            COLOR_MODE_BRIGHTNESS = 2,
            COLOR_MODE_WHITE = 7,
            COLOR_MODE_COLOR_TEMPERATURE = 11,
            COLOR_MODE_COLD_WARM_WHITE = 19,
            COLOR_MODE_RGB = 35,
            COLOR_MODE_RGB_WHITE = 39,
            COLOR_MODE_RGB_COLOR_TEMPERATURE = 47,
            COLOR_MODE_RGB_COLD_WARM_WHITE = 51,
        };

        enum ClimateMode : uint32_t
        {
            CLIMATE_MODE_OFF = 0,
            CLIMATE_MODE_HEAT_COOL = 1,
            CLIMATE_MODE_COOL = 2,
            CLIMATE_MODE_HEAT = 3,
            CLIMATE_MODE_FAN_ONLY = 4,
            CLIMATE_MODE_DRY = 5,
            CLIMATE_MODE_AUTO = 6,
        };
        enum ClimateFanMode : uint32_t
        {
            CLIMATE_FAN_ON = 0,
            CLIMATE_FAN_OFF = 1,
            CLIMATE_FAN_AUTO = 2,
            CLIMATE_FAN_LOW = 3,
            CLIMATE_FAN_MEDIUM = 4,
            CLIMATE_FAN_HIGH = 5,
            CLIMATE_FAN_MIDDLE = 6,
            CLIMATE_FAN_FOCUS = 7,
            CLIMATE_FAN_DIFFUSE = 8,
            CLIMATE_FAN_QUIET = 9,
        };
        enum ClimateSwingMode : uint32_t
        {
            CLIMATE_SWING_OFF = 0,
            CLIMATE_SWING_BOTH = 1,
            CLIMATE_SWING_VERTICAL = 2,
            CLIMATE_SWING_HORIZONTAL = 3,
        };
        enum ClimateAction : uint32_t
        {
            CLIMATE_ACTION_OFF = 0,
            CLIMATE_ACTION_COOLING = 2,
            CLIMATE_ACTION_HEATING = 3,
            CLIMATE_ACTION_IDLE = 4,
            CLIMATE_ACTION_DRYING = 5,
            CLIMATE_ACTION_FAN = 6,
        };
        enum ClimatePreset : uint32_t
        {
            CLIMATE_PRESET_NONE = 0,
            CLIMATE_PRESET_HOME = 1,
            CLIMATE_PRESET_AWAY = 2,
            CLIMATE_PRESET_BOOST = 3,
            CLIMATE_PRESET_COMFORT = 4,
            CLIMATE_PRESET_ECO = 5,
            CLIMATE_PRESET_SLEEP = 6,
            CLIMATE_PRESET_ACTIVITY = 7,
        };

    } //namespace enums

    class ESPNowMessage
    {
    public:
        uint32_t type{enums::MESSAGE_TYPE_UNKNOWN};
        uint32_t key{0};
    };

    class BinarySensorStateMessage : public ESPNowMessage
    {
    public:
        BinarySensorStateMessage() { this->type = enums::MESSAGE_TYPE_BINARY_SENSOR; }
        bool state{false};
    };

    class FanStateMessage : public ESPNowMessage
    {
    public:
        FanStateMessage() { this->type = enums::MESSAGE_TYPE_FAN; }
        bool state{false};
        bool has_oscillating{false};
        bool oscillating{false};
        bool has_speed{false};
        int32_t speed{0};
        bool has_direction{false};
        enums::FanDirection direction{};
    };

    class LightStateMessage : public ESPNowMessage
    {
    public:
        LightStateMessage() { this->type = enums::MESSAGE_TYPE_LIGHT; }
        bool state{false};
        float brightness{0.0f};
        enums::ColorMode color_mode{};
        float color_brightness{0.0f};
        float red{0.0f};
        float green{0.0f};
        float blue{0.0f};
        float white{0.0f};
        float color_temperature{0.0f};
        float cold_white{0.0f};
        float warm_white{0.0f};
        std::string effect{};
    };

    class SensorStateMessage : public ESPNowMessage
    {
    public:
        SensorStateMessage() { this->type = enums::MESSAGE_TYPE_SENSOR; }
        float state{0.0f};
    };

    class TextSensorStateMessage : public ESPNowMessage
    {
    public:
        TextSensorStateMessage() { this->type = enums::MESSAGE_TYPE_TEXT_SENSOR; }
        std::string state{};
    };

    class SwitchStateMessage : public ESPNowMessage
    {
    public:
        SwitchStateMessage() { this->type = enums::MESSAGE_TYPE_BUTTON; }
        bool state{false};
    };

    class ButtonStateMessage : public ESPNowMessage
    {
    public:
        ButtonStateMessage() { this->type = enums::MESSAGE_TYPE_BUTTON; }
    };

    class ClimateStateMessage : public ESPNowMessage
    {
    public:
        ClimateStateMessage() { this->type = enums::MESSAGE_TYPE_CLIMATE; }
        enums::ClimateMode mode{};
        bool has_target_temperature{false};
        float target_temperature{0.0f};
        bool has_two_point_target_temperature{false};
        float target_temperature_low{0.0f};
        float target_temperature_high{0.0f};
        bool has_fan_mode{false};
        enums::ClimateFanMode fan_mode{};
        bool has_custom_fan_mode{false};
        std::string custom_fan_mode{};
        bool has_preset{false};
        enums::ClimatePreset preset{};
        bool has_custom_preset{false};
        std::string custom_preset{};
        bool has_swing_mode{false};
        enums::ClimateSwingMode swing_mode{};
    };
}