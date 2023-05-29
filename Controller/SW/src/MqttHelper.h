#pragma once

#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoQueue.h>
#include <ArduinoJson.h>

#include "LogHelper.h"


class MqttHelper
{
public:
    MqttHelper(String ip, const int port, String id, LogHelper &logHelper, WiFiClient &wifiClient, uint16_t bufferSize);
    bool connect();
    bool subscribe(String topic);
    bool publish(String topic, String message);
    bool hasMessage();
    String getNextMessage();
    void loop();

private:
    uint16_t bufferSize;
    String ip;
    const int port;
    String id;
    LogHelper &logHelper;
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    ArduinoQueue<String> messageQueue;
};