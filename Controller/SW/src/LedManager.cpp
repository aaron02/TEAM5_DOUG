#include "LedManager.h"

void LedManager::initialize()
{
    pinMode(Led::LED_MQTT, OUTPUT);
    pinMode(Led::LED_WIFI, OUTPUT);
    pinMode(Led::LED_ERROR, OUTPUT);
    pinMode(Led::LED_STATUS, OUTPUT);

    digitalWrite(Led::LED_MQTT, false);
    digitalWrite(Led::LED_WIFI, false);
    digitalWrite(Led::LED_ERROR, false);
    digitalWrite(Led::LED_STATUS, false);
}

void LedManager::setLedState(Led led, bool state)
{
    digitalWrite(led, state);
}