#include "MqttManager.h"

// Initialize static variables
WiFiClient MqttManager::wifiClient;
PubSubClient MqttManager::mqttClient(MqttManager::wifiClient);
std::queue<MqttApplicationRequest> MqttManager::applicationRequestQueue;
bool MqttManager::hasOrderFlag = false;

bool MqttManager::initialize(std::string WifiSsid, std::string WifiPassword, unsigned long wifiConectionTimeout_ms, IPAddress mqttServerIP, int mqttServerPort, std::string mqttClientID, uint16_t mqttMaxBufferSize, unsigned long mqttConectionTimeout_ms)
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Connect to wifi with SSID: \"" + WifiSsid + "\" and password: \"" + WifiPassword + "\" with a timeout of " + std::to_string(wifiConectionTimeout_ms) + "ms");

    // Start wifi connection
    WiFi.begin(WifiSsid.c_str(), WifiPassword.c_str());

    // Wait for wifi connection with timeout
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < wifiConectionTimeout_ms)
    {
    }

    // Check if wifi connection is established
    if (WiFi.status() != WL_CONNECTED)
    {
        // Write log message
        Log::println(LogType::LOG_TYPE_ERROR, "MqttManager", "Wifi connection failed");

        return false;
    }

    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Wifi connection established");

    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Connect to MQTT server with a timeout");

    // Setup MQTT connection
    mqttClient.setServer(mqttServerIP, mqttServerPort);
    mqttClient.setCallback(mqttCallback);

    // Wait for MQTT connection with timeout
    startTime = millis();
    while (!mqttClient.connected() && millis() - startTime < mqttConectionTimeout_ms)
    {
        // Try to connect to the MQTT server
        mqttClient.connect(mqttClientID.c_str());
    }

    // Check if MQTT connection is established
    if (!mqttClient.connected())
    {
        // Write log message
        Log::println(LogType::LOG_TYPE_ERROR, "MqttManager", "MQTT connection failed");

        return false;
    }

    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "MQTT connection established");

    // Set MQTT buffer size
    mqttClient.setBufferSize(mqttMaxBufferSize);

    // Unsubscribe from all topics
    unsubscribeAllTopics();

    return true;
}

void MqttManager::keepAlive()
{
    // Call loop function to keep the MQTT connection alive
    mqttClient.loop();
}

void MqttManager::sendCurrentDeliveryId(std::string currentDeliveryId)
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Send current delivery ID: \"" + currentDeliveryId + "\"");

    publishMessage("Robots/" + getRobotUuid() + "/From/Status/CurrentDeliveryId", currentDeliveryId);
}

void MqttManager::sendCurrentDeliveryStep(std::string currentDeliveryId, int currentDeliveryStep)
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Send current delivery step: \"" + std::to_string(currentDeliveryStep) + "\" for delivery ID: \"" + currentDeliveryId + "\"");
    // Create JSON message

    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["deliveryId"] = currentDeliveryId;
    jsonDoc["deliveryStep"] = currentDeliveryStep;

    // Publish message
    publishMessage("Robots/" + getRobotUuid() + "/From/Status/CurrentDeliveryStep", jsonDoc);
}

void MqttManager::sendCurrentBatteryState(int currentBatteryState)
{
    publishMessage("Robots/" + getRobotUuid() + "/From/Status/BatteryChargePct", std::to_string(currentBatteryState));
}

void MqttManager::sendDeliveryDone()
{
    publishMessage("Robots/" + getRobotUuid() + "/From/Status/DeliveryDone", true);
}

void MqttManager::sendCurrentPosition(Coordinates currentPosition)
{
    // Create JSON message
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["x"] = currentPosition.x;
    jsonDoc["y"] = currentPosition.y;

    // Publish message
    publishMessage("Robots/" + getRobotUuid() + "/From/Status/CurrentPosition", jsonDoc);
}

void MqttManager::requestOrder()
{
    // Unsubscribe from the order topic
    unsubscribeTopic("Robots/" + getRobotUuid() + "/To/DeliveryOrder");

    // Reset the flag that a new order is available
    hasOrderFlag = false;

    // Subscribe to the order topic
    subscribeTopic("Robots/" + getRobotUuid() + "/To/DeliveryOrder");

    // Request an order
    MqttManager::publishMessage("Robots/" + getRobotUuid() + "/From/Requests/GiveMeAnOrder", true);
}

bool MqttManager::hasOrder()
{
    return hasOrderFlag;
}

bool MqttManager::hasApplicationRequest()
{
    return !applicationRequestQueue.empty();
}

MqttApplicationRequest MqttManager::getApplicationRequest()
{
    // Store the first application request from the queue
    MqttApplicationRequest applicationRequest = applicationRequestQueue.front();

    // Pop the first application request from the queue
    applicationRequestQueue.pop();

    // Return the application request
    return applicationRequest;
}

std::string MqttManager::getRobotUuid()
{
    // Get the wifi MAC-Address and convert it to a string
    std::string macAddress = WiFi.macAddress().c_str();

    // Convert the MAC-Address to lower case
    std::transform(macAddress.begin(), macAddress.end(), macAddress.begin(), ::tolower);

    // Remove the colons from the MAC-Address
    macAddress.erase(std::remove(macAddress.begin(), macAddress.end(), ':'), macAddress.end());

    // Return the robot UUID
    return "583665f0-0dca-4b75-9358-" + macAddress;
}

void MqttManager::publishMessage(std::string topic, std::string message)
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Publish message: \"" + message + "\" to topic: \"" + topic + "\"");

    // Publish the message
    mqttClient.publish(topic.c_str(), message.c_str());
}

void MqttManager::publishMessage(std::string topic, DynamicJsonDocument &doc)
{
    // Serialize the JSON document to a string
    std::string message;
    serializeJson(doc, message);

    // Publish the message
    publishMessage(topic, message);
}

void MqttManager::publishMessage(std::string topic, bool value)
{
    // Convert the boolean value to a string
    std::string stringValue = value ? "1" : "0";

    // Publish the message
    publishMessage(topic, stringValue);
}

void MqttManager::subscribeTopic(std::string topic)
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Subscribe to topic: \"" + topic + "\"");

    mqttClient.subscribe(topic.c_str());
}

void MqttManager::unsubscribeTopic(std::string topic)
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Unsubscribe from topic: \"" + topic + "\"");

    mqttClient.unsubscribe(topic.c_str());
}

void MqttManager::mqttCallback(char *topic, byte *payload, unsigned int length)
{
    // Convert the payload to a string
    std::string payloadString(reinterpret_cast<char *>(payload), length);

    // Create MQTT message opbject
    MqttMessage mqttMessage = {std::string(topic), payloadString};

    // Check if the topic is a order
    if (mqttMessage.topic == "Robots/" + getRobotUuid() + "/To/DeliveryOrder")
    {
        // Write log message
        Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Received new order");

        // Parse the order
        OrderManager::parse(mqttMessage.payload);

        // Set the flag that a new order is available
        hasOrderFlag = true;
    }

    // Check if the topic is a application request for a status update
    if (mqttMessage.topic == "Robots/" + getRobotUuid() + "/To/Requests/StatusUpdate")
    {
        // Write log message
        Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Received application request for a status update");

        applicationRequestQueue.push(MqttApplicationRequest::MqttApplicationRequestStatusUpdate);
    }

    // Check if the topic is a application request to return to base
    if (mqttMessage.topic == "Robots/" + getRobotUuid() + "/To/Requests/ReturnToBase")
    {
        // Write log message
        Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Received application request to return to base");

        applicationRequestQueue.push(MqttApplicationRequest::MqttApplicationRequestReturnToBase);
    }
}

void MqttManager::unsubscribeAllTopics()
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "MqttManager", "Unsubscribe from all topics");

    mqttClient.unsubscribe("#");
}