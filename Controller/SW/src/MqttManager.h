#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include <queue>
#include <ArduinoJson.h>

#include "Log.h"
#include "Coordinates.h"
#include "MqttMessage.h"
#include "OrderManager.h"

// The different Request types from the application
enum MqttApplicationRequest
{
    MqttApplicationRequestStatusUpdate,
    MqttApplicationRequestReturnToBase
};

/// @brief This class is used to manage the MQTT connection
class MqttManager
{
public:
    /// @brief Initialize the MQTT manager
    /// @param mqttMaxBufferSize The maximum receive buffer size for MQTT messages
    /// @return  True if the initialization was successful otherwise false
    static bool initialize(std::string WifiSsid, std::string WifiPassword, unsigned long wifiConectionTimeout_ms, IPAddress mqttServerIP, int mqttServerPort, std::string mqttClientID, uint16_t mqttMaxBufferSize, unsigned long mqttConectionTimeout_ms);

    /// @brief Keep the MQTT connection alive
    static void keepAlive();

    /// @brief Unsubscribe from all topics
    static void unsubscribeAllTopics();

    /// @brief Send uptade on the current delivery id
    /// @param currentDeliveryId Current delivery ID
    static void sendCurrentDeliveryId(std::string currentDeliveryId);

    /// @brief Send sptade on the current delivery step and delivery id
    static void sendCurrentDeliveryStep(std::string currentDeliveryId, int currentDeliveryStep);

    /// @brief Send uptade on the current battery state
    static void sendCurrentBatteryState(int currentBatteryState);

    /// @brief Send the flag delivery done
    static void sendDeliveryDone();

    /// @brief Send uptade on the current position
    static void sendCurrentPosition(Coordinates currentPosition);

    /// @brief Subscribe to the order topic and delete all orders in the queue and then request an new order by sending the GiveMeAnOrder flag
    static void requestOrder();

    /// @brief Check if there is an order in the queue
    /// @return True if there is an order in the queue otherwise false
    static bool hasOrder();

    /// @brief Check if there is an application request in the queue
    /// @return True if there is an application request in the queue otherwise false
    static bool hasApplicationRequest();

    /// @brief Get the first application request in the queue
    static MqttApplicationRequest getApplicationRequest();

private:
    /// @brief Application request queue used to store incoming application requests
    static std::queue<MqttApplicationRequest> applicationRequestQueue;

    /// @brief WiFi client used to connect to the MQTT server
    static WiFiClient wifiClient;

    /// @brief MQTT client used to communicate with the MQTT server
    static PubSubClient mqttClient;

    /// @brief Flag used to indicate if there is an order
    static bool hasOrderFlag;

    /// @brief Returns the UUID of the robot generated from the MAC-Address
    /// @return UUID in the format xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    static std::string getRobotUuid();

    /// @brief  Publish a message to the MQTT server
    static void publishMessage(std::string topic, std::string message);
    static void publishMessage(std::string topic, DynamicJsonDocument &doc);
    static void publishMessage(std::string topic, bool value);

    /// @brief Subscribe to a topic
    static void subscribeTopic(std::string topic);

    /// @brief Unsubscribe from a topic
    static void unsubscribeTopic(std::string topic);

    /// @brief Callback function for MQTT messages: Messages get parsed and added to the queues
    static void mqttCallback(char *topic, byte *payload, unsigned int length);
};