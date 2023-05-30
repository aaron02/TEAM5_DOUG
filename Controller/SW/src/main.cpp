#include <Arduino.h>
#include "WiFiHelper.h"
#include "MqttHelper.h"
#include <ArduinoJson.h>
#include "Waypoint.h"
#include "RobotHelper.h"
#include <HardwareSerial.h>

// ===================== CONSTANTS =====================
const String IP = "91.121.93.94";
const String SSID = "McDonalds";
const String PASSWORD = "burgerking";
const String MQTT_DEVICE_ID = "Dough";
const int MQTT_PORT = 1883;

// ===================== OBJECTS =====================
WiFiClient wifiClient;
LogHelper logHelper(Serial, 115200);
WiFiHelper wifiHelper(SSID, PASSWORD, logHelper, wifiClient);
MqttHelper mqttHelper(IP, MQTT_PORT, MQTT_DEVICE_ID, logHelper, wifiClient, 10000);
RobotHelper robotHelper(115200);
String topicDeliveryOrder;

// ===================== SETUP =====================
void setup()
{
    Serial1.begin(115200);
    Serial1.println("jhkjdsh");

    logHelper.connect();
    wifiHelper.connect(100000);
    mqttHelper.connect();

    topicDeliveryOrder = "Robots/" + wifiHelper.getMacAddress() + "/To/DeliveryOrder";

    mqttHelper.subscribe(topicDeliveryOrder);
    robotHelper.connect();
}

// ===================== LOOP =====================
void loop()
{
    mqttHelper.loop();

    if (mqttHelper.hasMessage())
    {
        MqttMessage message = mqttHelper.getNextMessage();

        DynamicJsonDocument doc(10000);
        deserializeJson(doc, message.payload);

        if (doc["deliverySteps"].is<JsonArray>())
        {
            JsonArray array = doc["deliverySteps"].as<JsonArray>();
            size_t size = array.size();

            for (int i = 0; i < size; i++)
            {
                Coordinates coordinates;
                coordinates.x = doc["deliverySteps"][i]["coordinates"]["x"].as<int>();
                coordinates.y = doc["deliverySteps"][i]["coordinates"]["y"].as<int>();

                Waypoint waypoint(WAYPOINT, coordinates, doc["deliverySteps"][i]["id"]);

                robotHelper.addWaypointToQueue(waypoint);
            }

            while (robotHelper.hasWaypointInQueue())
            {
                Waypoint waypoint = robotHelper.popWaypointFromQueue();   
                             logHelper.println(LOG_LEVEL_LOG, "Send new waypoint: x = " + String(waypoint.GetCoordinates().x) + " y = " + String(waypoint.GetCoordinates().y));
                robotHelper.setNextWaypoint(waypoint);
                while (!robotHelper.readyForNextWaypoint())
                {
                    delay(100);
                }
                  logHelper.println(LOG_LEVEL_LOG, "Arrived!");
            }
        }
    }
}