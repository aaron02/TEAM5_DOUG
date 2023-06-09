#include "MqttManager.h"

uint16_t MqttManager::messageBufferSize;
IPAddress MqttManager::mqttServerIP;
int MqttManager::mqttServerPort;
std::string MqttManager::mqttClientID;
std::string MqttManager::wifiSsid;
std::string MqttManager::wifiPassword;
WiFiClient MqttManager::wifiClient;
PubSubClient MqttManager::mqttClient(MqttManager::wifiClient);
std::queue<MqttMessage> MqttManager::messageQueue;

bool MqttManager::connect(IPAddress serverIP, int serverPort, std::string clientID, std::string ssid, std::string password, uint16_t bufferSize)
{
    MqttManager::mqttServerIP = serverIP;
    MqttManager::mqttServerPort = serverPort;
    MqttManager::mqttClientID = clientID;
    MqttManager::wifiSsid = ssid;
    MqttManager::wifiPassword = password;
    MqttManager::messageBufferSize = bufferSize;

    mqttClient.setServer(serverIP, serverPort);
    mqttClient.setCallback(mqttCallback);

    Log::println(LogType::LOG_TYPE_LOG, "Connecting to Wi-Fi with timeout of 10s...");
    WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());

    unsigned long startMillis = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startMillis) < 10000)
    {
        delay(500);
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Log::println(LogType::LOG_TYPE_ERROR, "Failed to connect to Wi-Fi");
        return false;
    }

    Log::println(LogType::LOG_TYPE_LOG, "Connected to Wi-Fi");

    Log::println(LogType::LOG_TYPE_LOG, "Connecting to MQTT Server...");
    unsigned long mqttStartMillis = millis();
    while (!mqttClient.connected() && (millis() - mqttStartMillis) < 5000)
    {
        if (mqttClient.connect(mqttClientID.c_str()))
        {
            Log::println(LogType::LOG_TYPE_LOG, "Connected to MQTT Server");
        }
        else
        {
            Log::println(LogType::LOG_TYPE_ERROR, "Failed to connect to MQTT Server, retrying...");
            delay(1000);
        }
    }

    if (!mqttClient.connected())
    {
        Log::println(LogType::LOG_TYPE_ERROR, "Failed to connect to MQTT Server within the timeout period");
        return false;
    }

    if (!mqttClient.setBufferSize(messageBufferSize))
    {
        Log::println(LogType::LOG_TYPE_ERROR, "Failed to set MQTT buffer size to: " + std::to_string(messageBufferSize));
        return false;
    }

    mqttClient.loop();
    return true;
}

bool MqttManager::subscribeTopic(std::string topic)
{
    Log::println(LogType::LOG_TYPE_LOG, "Subscribing to \"" + std::string(topic) + "\"...");
    if (!mqttClient.subscribe(topic.c_str()))
    {
        Log::println(LogType::LOG_TYPE_ERROR, "Failed to subscribe");
        return false;
    }

    Log::println(LogType::LOG_TYPE_LOG, "Subscription successful");
    return true;
}

bool MqttManager::unsubscribeTopic(std::string topic)
{
    Log::println(LogType::LOG_TYPE_LOG, "Unsubscribing from \"" + std::string(topic) + "\"...");
    if (!mqttClient.unsubscribe(topic.c_str()))
    {
        Log::println(LogType::LOG_TYPE_ERROR, "Failed to unsubscribe");
        return false;
    }

    Log::println(LogType::LOG_TYPE_LOG, "Unsubscribe successful");
    return true;
}

bool MqttManager::unsubscribeAllTopics()
{
    Log::println(LogType::LOG_TYPE_LOG, "Unsubscribing from all topics...");
    if (!mqttClient.unsubscribe("#"))
    {
        Log::println(LogType::LOG_TYPE_ERROR, "Failed to unsubscribe from all topics");
        return false;
    }

    Log::println(LogType::LOG_TYPE_LOG, "Unsubscribe from all topics successful");
    return true;
}

bool MqttManager::isConnected()
{
    return mqttClient.connected();
}

std::string MqttManager::getMacAddress()
{
    //  return std::string(WiFi.macAddress().c_str());
    return "ce29a244-acb0-4dde-9497-24eda5b46b11";
}

bool MqttManager::publishMessage(std::string topic, std::string message)
{
    Log::println(LogType::LOG_TYPE_LOG, "Publishing to \"" + std::string(topic) + "\"...");
    if (!mqttClient.publish(topic.c_str(), message.c_str()))
    {
        Log::println(LogType::LOG_TYPE_ERROR, "Failed to publish");
        return false;
    }

    Log::println(LogType::LOG_TYPE_LOG, "Publish successful");
    return true;
}

bool MqttManager::publishMessage(std::string topic, bool value)
{
    std::string stringValue = value ? "1" : "0";

    Log::println(LogType::LOG_TYPE_LOG, "Publishing \"" + stringValue + "\" to \"" + std::string(topic) + "\"...");
    if (!mqttClient.publish(topic.c_str(), stringValue.c_str()))
    {
        Log::println(LogType::LOG_TYPE_ERROR, "Failed to publish");
        return false;
    }

    Log::println(LogType::LOG_TYPE_LOG, "Publish successful");
    return true;
}

bool MqttManager::hasIncomingMessage()
{
    return !messageQueue.empty();
}

MqttMessage MqttManager::getNextMessage()
{
    MqttMessage mqttMessage = messageQueue.front();
    messageQueue.pop();
    return mqttMessage;
}

void MqttManager::run()
{
    mqttClient.loop();
}

void MqttManager::clearMessageQueue()
{
    while (!messageQueue.empty())
    {
        messageQueue.pop();
    }
}

void MqttManager::mqttCallback(char *topic, byte *payload, unsigned int length)
{
    std::string payloadString(reinterpret_cast<char *>(payload), length);
    MqttMessage mqttMessage = {std::string(topic), payloadString};
    messageQueue.push(mqttMessage);
}
