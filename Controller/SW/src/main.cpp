#include <Arduino.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

// ------------------------- Constants -------------------------
const String SSID = "McDonalds";
const String PASSWORD = "burgerking";
const String TEST_SERVER_URL = "test.mosquitto.org";
const String MQTT_DEVICE_ID = "Dough";
const int MQTT_PORT = 1883;
const unsigned long SERIAL_BAUD_DEBUG = 115200;
const unsigned long SERIAL_BAUD_DRIVER = 115200;
const String MQTT_SUBSCRIPTION_TOPICS[] = {
    "topic1",
    "topic2",
    "topic3"};

// ------------------------- Objects -------------------------
WiFiClient WifiClient;
PubSubClient MqttClient(WifiClient);
HardwareSerial SerialDebug(0);
HardwareSerial SerialDriver(1);

// ------------------------- Variables -------------------------
String LastErrorMessage = "";

// ------------------------- Enums -------------------------
enum RobotState
{
    RobotStateError,
    RobotStateBusy,
    RobotStateFinished
};

// ------------------------- Functions -------------------------

void MqttCallback(char *topic, byte *payload, unsigned int length)
{
    SerialDebug.print("Neue Nachricht empfangen auf Thema: ");
    SerialDebug.println(topic);

    // Parse the payload using ArduinoJSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error)
    {
        SerialDebug.print("Fehler beim Analysieren der JSON-Nachricht: ");
        SerialDebug.println(error.c_str());
        return;
    }

    // Format the parsed JSON document for pretty printing
    String formattedJson;
    serializeJsonPretty(doc, formattedJson);

    // Print the formatted JSON to the SerialDebug output
    SerialDebug.print("Inhalt:\n");
    SerialDebug.println(formattedJson);
}

bool InitDebugUart(String &errorMessage)
{
    // Reset error message
    errorMessage = "";

    // Init port
    SerialDebug.begin(SERIAL_BAUD_DEBUG);

    // Check if port is ready
    if (!SerialDebug)
    {
        errorMessage = "Could not open Port for debug UART";
        return false;
    }

    return true;
}

bool InitDriverUart(String &errorMessage)
{
    // Reset error message
    errorMessage = "";

    // Init port
    SerialDriver.begin(SERIAL_BAUD_DEBUG);

    // Check if port is ready
    if (!SerialDriver)
    {
        errorMessage = "Could not open Port for debug UART";
        return false;
    }

    return true;
}

bool InitWifi(unsigned int connectionTimeout_ms, String &errorMessage)
{
    // Reset error message
    errorMessage = "";

    // Connect to Wifi
    WiFi.begin(SSID.c_str(), PASSWORD.c_str());

    // Wait for connection with timeout
    unsigned long startMillis = millis();
    bool finished = false;
    while ((millis() - startMillis) < connectionTimeout_ms && WiFi.status() != WL_CONNECTED)
    {
    }

    // Check Connection
    if (WiFi.status() != WL_CONNECTED)
    {
        errorMessage = "Could not connect to Wifi";
        return false;
    }

    return true;
}

bool InitMqtt(String &errorMessage)
{
    // Reset error message
    errorMessage = "";

    // Resolve MQTT server URL to IP address
    IPAddress mqttServerIP;
    WiFi.hostByName(TEST_SERVER_URL.c_str(), mqttServerIP);
    MqttClient.setServer(mqttServerIP, MQTT_PORT);

    // Connect to MQTT server
    if (!MqttClient.connect(MQTT_DEVICE_ID.c_str()))
    {
        errorMessage = "Could not connect to MQTT broker: " + MqttClient.state();
        return false;
    }

    // Set callback function
    MqttClient.setCallback(MqttCallback);

    return true;
}

// ------------------------- Setup -------------------------
void setup()
{
    if (!InitDebugUart(LastErrorMessage))
    {
        while (true)
        {
        }
    }

    if (!InitDriverUart(LastErrorMessage))
    {
        // Print error message
        SerialDebug.println("Error: " + LastErrorMessage);

        // Loop forever
        while (true)
        {
        }
    }

    if (!InitWifi(10000, LastErrorMessage))
    {
        // Print error message
        SerialDebug.println("Error: " + LastErrorMessage);

        // Loop forever
        while (true)
        {
        }
    }

    if (!InitMqtt(LastErrorMessage))
    {
        // Print error message
        SerialDebug.println("Error: " + LastErrorMessage);

        // Loop forever
        while (true)
        {
        }
    }

    MqttClient.publish("Robots", "Gooooo");
    MqttClient.subscribe("Robots");
}

// ------------------------- Loop -------------------------
void loop()
{
    MqttClient.loop();
}