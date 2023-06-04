#include "MqttHelper.h"

MqttHelper::MqttHelper(String ip, const int port, String id, LogHelper &logHelper, WiFiClient &wifiClient, uint16_t bufferSize)
    : ip(ip), port(port), id(id), logHelper(logHelper), wifiClient(wifiClient), mqttClient(wifiClient), bufferSize(bufferSize)
{
}

bool MqttHelper::connect()
{
    logHelper.println(LOG_LEVEL_LOG, "Connecting to MQTT Server (" + ip + ")...");

    mqttClient.setServer(ip.c_str(), port);

    // Connect to MQTT server
    unsigned long startMillis = millis();
    while (!mqttClient.connected() && (millis() - startMillis) < 5000)
    {
        if (mqttClient.connect(id.c_str()))
        {
            logHelper.println(LOG_LEVEL_LOG, "Connected to MQTT Server");
        }
        else
        {
            logHelper.println(LOG_LEVEL_ERROR, "Failed to connect to MQTT Server, retrying...");
            delay(1000);
        }
    }

    if (!mqttClient.connected())
    {
        logHelper.println(LOG_LEVEL_ERROR, "Failed to connect to MQTT Server within the timeout period");
        return false;
    }

    // Set buffer size
    mqttClient.setBufferSize(bufferSize);

    // Set callback
    mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length)
                           {
                                String payloadString;
                                for (unsigned int i = 0; i < length; i++) 
                                {
                                    payloadString += (char)payload[i];
                                }
                                MqttMessage mqttMessage;
                                mqttMessage.topic = String(topic);
                                mqttMessage.payload = payloadString;
                                messageQueue.push(mqttMessage); });

    return true;
}

bool MqttHelper::subscribe(String topic)
{
    logHelper.println(LOG_LEVEL_LOG, "Subscring to \"" + topic + "\"...");

    if (!mqttClient.subscribe(topic.c_str()))
    {
        logHelper.println(LOG_LEVEL_ERROR, "Failed to subscribe");
        return false;
    }

    logHelper.println(LOG_LEVEL_LOG, "Subscring successful");
    return true;
}

bool MqttHelper::publish(String topic, String message)
{
    logHelper.println(LOG_LEVEL_LOG, "Publish to \"" + topic + "\"...");

    if (!mqttClient.publish(topic.c_str(), message.c_str()))
    {
        logHelper.println(LOG_LEVEL_ERROR, "Failed to publish");
        return false;
    }

    logHelper.println(LOG_LEVEL_LOG, "Publish successful");
    return true;
}

bool MqttHelper::hasMessage()
{
    return !messageQueue.empty();
}

MqttMessage MqttHelper::getNextMessage()
{
    MqttMessage mqttMessage = messageQueue.front();
    messageQueue.pop();
    return mqttMessage;
}

void MqttHelper::loop()
{
    mqttClient.loop();
}