#include <Arduino.h>
#include "WiFiHelper.h"
#include "MqttHelper.h"
#include <ArduinoJson.h>
#include "Waypoint.h"
#include "RobotHelper.h"
#include <HardwareSerial.h>

// ===================== ENUMS =====================
enum ProgramState
{
    ProgramStateInit,
    ProgramStateRecieveOrder,
    ProgramStateDeliverOrder,
    ProgramStateError
};

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
ProgramState programState = ProgramStateInit;

void changeState(ProgramState newState)
{
    String logMessage = "Change program state to ";
    switch (newState)
    {
    case ProgramState::ProgramStateInit:
        logMessage += "Init";
        break;

    case ProgramState::ProgramStateRecieveOrder:
        logMessage += "Init";
        break;

    case ProgramState::ProgramStateDeliverOrder:
        logMessage += "Deliver order";
        break;

    case ProgramState::ProgramStateError:
        logMessage += "Error";
        break;

    default:
        break;
    }

    logHelper.println(LOG_LEVEL_LOG, logMessage);
    programState = newState;
}

// ===================== SETUP =====================
void setup()
{
    programState = ProgramState::ProgramStateInit;

    topicDeliveryOrder = "Robots/" + wifiHelper.getMacAddress() + "/To/DeliveryOrder";

    mqttHelper.subscribe(topicDeliveryOrder);
    robotHelper.connect();
}

// ===================== LOOP =====================
void loop()
{
    switch (programState)
    {
    case ProgramState::ProgramStateInit:
        if (!logHelper.connect())
        {
            changeState(ProgramStateError);
        }
        if (!wifiHelper.connect(100000))
        {
            changeState(ProgramStateError);
        }
        if (!mqttHelper.connect())
        {
            changeState(ProgramStateError);
        }

        changeState(ProgramStateRecieveOrder);
        break;

    case ProgramState::ProgramStateRecieveOrder:
    
        /* code */
        break;

    case ProgramState::ProgramStateDeliverOrder:
        /* code */
        break;

    default:
        programState = ProgramState::ProgramStateError;
        break;
    }

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

                do
                {
                    delay(3000);
                } while (!robotHelper.readyForNextWaypoint());

                logHelper.println(LOG_LEVEL_LOG, "Arrived!");
            }
        }
    }
}