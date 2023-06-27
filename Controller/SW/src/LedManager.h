#pragma once

#include <Arduino.h>

enum Led
{
    LED_MQTT = 9,
    LED_WIFI = 10,
    LED_ERROR = 11,
    LED_STATUS = 12
};

class LedManager
{
public:
    /// @brief Init all pins and set them to low
    static void initialize();

    /// @brief Set led state
    static void setLedState(Led led, bool state);
};