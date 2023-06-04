#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <queue>
#include "Log.h"

struct MqttMessage
{
    String topic;
    String payload;
};

/**
 * @brief The MqttManager class provides functionality to manage MQTT communication.
 */
class MqttManager
{
public:
    /**
     * @brief Initializes the MqttManager with the specified parameters.
     * @param serverIP The IP address of the MQTT server.
     * @param serverPort The port number of the MQTT server.
     * @param clientID The client ID to be used for MQTT connection.
     * @param ssid The SSID of the Wi-Fi network to connect to.
     * @param password The password for the Wi-Fi network.
     * @param bufferSize The size of the message buffer.
     */
    static void initialize(const char *serverIP, int serverPort, const char *clientID, const char *ssid, const char *password, uint16_t bufferSize);

    /**
     * @brief Connects to the Wi-Fi network and the MQTT server.
     * @return `true` if the connection is successful, `false` otherwise.
     */
    static bool connect();

    /**
     * @brief Subscribes to the specified topic.
     * @param topic The topic to subscribe to.
     * @return `true` if the subscription is successful, `false` otherwise.
     */
    static bool subscribeTopic(const char *topic);

    /**
     * @brief Unsubscribes from the specified topic.
     * @param topic The topic to unsubscribe from.
     * @return `true` if the unsubscription is successful, `false` otherwise.
     */
    static bool unsubscribeTopic(const char *topic);

    /**
     * @brief Unsubscribes from all topics.
     * @return `true` if the unsubscription is successful, `false` otherwise.
     */
    static bool unsubscribeAllTopics();

/**
 * @brief Checks if the MQTT client is currently connected.
 * @return `true` if connected, `false` otherwise.
 */
static bool isConnected();

    /**
     * @brief Publishes a message to the specified topic.
     * @param topic The topic to publish the message to.
     * @param message The message to be published.
     * @return `true` if the publishing is successful, `false` otherwise.
     */
    static bool publishMessage(const char *topic, const char *message);

    /**
     * @brief Checks if there is an incoming message in the message queue.
     * @return `true` if there is an incoming message, `false` otherwise.
     */
    static bool hasIncomingMessage();

    /**
     * @brief Retrieves the next incoming message from the message queue.
     * @return The next incoming message.
     */
    static MqttMessage getNextMessage();

    /**
     * @brief Runs the MQTT client loop to handle incoming messages.
     */
    static void run();

    /**
     * @brief Clears the message queue.
     */
    static void clearMessageQueue();

private:
    static uint16_t messageBufferSize;           /**< The size of the message buffer. */
    static const char *mqttServerIP;             /**< The IP address of the MQTT server. */
    static int mqttServerPort;                   /**< The port number of the MQTT server. */
    static const char *mqttClientID;             /**< The client ID for MQTT connection. */
    static const char *wifiSsid;                 /**< The SSID of the Wi-Fi network. */
    static const char *wifiPassword;             /**< The password for the Wi-Fi network. */
    static WiFiClient wifiClient;                /**< The Wi-Fi client. */
    static PubSubClient mqttClient;              /**< The MQTT client. */
    static std::queue<MqttMessage> messageQueue; /**< The message queue. */

    /**
     * @brief The callback function called when an MQTT message is received.
     * @param topic The topic of the received message.
     * @param payload The payload of the received message.
     * @param length The length of the payload.
     */
    static void mqttCallback(char *topic, byte *payload, unsigned int length);
};