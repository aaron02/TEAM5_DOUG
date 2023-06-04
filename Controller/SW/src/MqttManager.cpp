#include "MqttManager.h"

uint16_t MqttManager::messageBufferSize;
const char* MqttManager::mqttServerIP;
int MqttManager::mqttServerPort;
const char* MqttManager::mqttClientID;
const char* MqttManager::wifiSsid;
const char* MqttManager::wifiPassword;
WiFiClient MqttManager::wifiClient;
PubSubClient MqttManager::mqttClient(MqttManager::wifiClient);
std::queue<MqttMessage> MqttManager::messageQueue;

void MqttManager::initialize(const char* serverIP, int serverPort, const char* clientID, const char* ssid, const char* password, uint16_t bufferSize)
{
    MqttManager::mqttServerIP = serverIP;
    MqttManager::mqttServerPort = serverPort;
    MqttManager::mqttClientID = clientID;
    MqttManager::wifiSsid = ssid;
    MqttManager::wifiPassword = password;
    MqttManager::messageBufferSize = bufferSize;

    mqttClient.setServer(serverIP, serverPort);
    mqttClient.setCallback(mqttCallback);
}

bool MqttManager::connect()
{
    Log::println(LogType::LOG_TYPE_LOG, "Connecting to Wi-Fi with timeout of 10s...");
    WiFi.begin(wifiSsid, wifiPassword);

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
        if (mqttClient.connect(mqttClientID))
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
        Log::println(LogType::LOG_TYPE_ERROR, "Failed to set MQTT buffer size to: " + String(messageBufferSize));
        return false;
    }

    mqttClient.loop();
    return true;
}

bool MqttManager::subscribeTopic(const char* topic)
{
    Log::println(LogType::LOG_TYPE_LOG, "Subscribing to \"" + String(topic) + "\"...");
    if (!mqttClient.subscribe(topic))
    {
        Log::println(LogType::LOG_TYPE_ERROR, "Failed to subscribe");
        return false;
    }

    Log::println(LogType::LOG_TYPE_LOG, "Subscription successful");
    return true;
}

bool MqttManager::unsubscribeTopic(const char* topic)
{
    Log::println(LogType::LOG_TYPE_LOG, "Unsubscribing from \"" + String(topic) + "\"...");
    if (!mqttClient.unsubscribe(topic))
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

bool MqttManager::publishMessage(const char* topic, const char* message)
{
    Log::println(LogType::LOG_TYPE_LOG, "Publishing to \"" + String(topic) + "\"...");
    if (!mqttClient.publish(topic, message))
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

void MqttManager::mqttCallback(char* topic, byte* payload, unsigned int length)
{
    String payloadString(reinterpret_cast<char*>(payload), length);
    MqttMessage mqttMessage = {String(topic), payloadString};
    messageQueue.push(mqttMessage);
}
