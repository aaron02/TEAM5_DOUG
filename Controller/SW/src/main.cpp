#include <Arduino.h>
#include <MqttManager.h>
#include <ArduinoJson.h>
#include "RobotManager.h"
#include "Log.h"
#include <HardwareSerial.h>
#include "Order.h"
#include <array>

// ===================== ENUMS =====================
enum ProgramState
{
    ProgramStateInit,
    ProgramStateRecieveOrder,
    ProgramStateDeliverOrder,
    ProgramStateError
};

// ===================== CONSTANTS =====================
const std::string WIFI_SSID = "McDonalds";
const std::string WIFI_PASSWORD = "burgerking";
const IPAddress MQTT_IP(91, 121, 93, 94);
const std::string MQTT_ID = "Dough";
const int MQTT_PORT = 1883;

// ===================== OBJECTS =====================
ProgramState programState = ProgramStateInit;
bool firstRun = true;
Order order;
std::array<std::string, 4> storageArray;

TaskHandle_t mqttRunTask;

void changeState(ProgramState newState)
{
    std::string logMessage = "Change program state to ";
    switch (newState)
    {
    case ProgramState::ProgramStateInit:
        logMessage += "Init";
        break;

    case ProgramState::ProgramStateRecieveOrder:
        logMessage += "Recieve order";
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

    Log::println(LogType::LOG_TYPE_LOG, logMessage);
    programState = newState;
    firstRun = true;
}

void mqttRun(void *pvParameters)
{
    MqttManager::run();
}

// ===================== SETUP =====================
void setup()
{
    changeState(ProgramState::ProgramStateInit);

    xTaskCreatePinnedToCore(
        mqttRun,      // Task function
        "MyTask",     // Task name
        10000,        // Stack size (bytes)
        NULL,         // Task parameters
        1,            // Task priority
        &mqttRunTask, // Task handle
        1             // Task core (1 for core 1)
    );
}

// ===================== LOOP =====================
void loop()
{
    switch (programState)
    {
    case ProgramState::ProgramStateInit:
        Log::initialize(115200);

        if (!MqttManager::connect(MQTT_IP, MQTT_PORT, MQTT_ID, WIFI_SSID, WIFI_PASSWORD, 10000))
        {
            changeState(ProgramStateError);
        }

        vTaskResume(mqttRunTask);

        if (!RobotManager::connect(115200))
        {
            changeState(ProgramStateError);
        }

        changeState(ProgramStateRecieveOrder);
        break;

    case ProgramState::ProgramStateRecieveOrder:
        if (firstRun)
        {
            MqttManager::unsubscribeAllTopics();
            MqttManager::subscribeTopic("Robots/" + MqttManager::getMacAddress() + "/To/DeliveryOrder");
            MqttManager::publishMessage("Robots/" + MqttManager::getMacAddress() + "/From/Requests/GiveMeAnOrder", true);

            firstRun = false;
        }

        if (MqttManager::hasIncomingMessage())
        {
            MqttMessage newMessage = MqttManager::getNextMessage();
            order.parse(newMessage.payload);

            MqttManager::unsubscribeTopic("Robots/" + MqttManager::getMacAddress() + "/To/DeliveryOrder");
            changeState(ProgramStateDeliverOrder);
        }

        MqttManager::run();
        break;

    case ProgramState::ProgramStateDeliverOrder:
        if (firstRun)
        {
            MqttManager::publishMessage("Robots/" + MqttManager::getMacAddress() + "/From/Status/CurrentDeliveryId", order.getdeliveryId());
            MqttManager::publishMessage("Robots/" + MqttManager::getMacAddress() + "/From/Status/DeliveryDone", false);

            RobotManager::abortDriving();
            while (RobotManager::getDrivingState() != RobotDrivingState::RobotDrivingStateFinished)
            {
            }

            RobotManager::setArmPosition(RobotArmPosition::RobotArmPositionReady);
            while (RobotManager::getArmState() != RobotArmState::RobotArmStateFinished)
            {
            }

            firstRun = false;
        }

        while (order.hasDeliveryStep())
        {
            DeliveryStep nextDeliveryStep = order.getNextDeliveryStep();

            std::string mqttMessage;
            DynamicJsonDocument doc(1024);
            doc["deliveryId"] = order.getdeliveryId();
            doc["deliveryStep"] = nextDeliveryStep.id;
            serializeJson(doc, mqttMessage);
            doc.clear();
            MqttManager::publishMessage("Robots/" + MqttManager::getMacAddress() + "/From/Status/CurrentDeliveryStep", mqttMessage);

            RobotManager::setDrivingWaypoint(nextDeliveryStep.coordinates);
            while (RobotManager::getDrivingState() != RobotDrivingState::RobotDrivingStateFinished)
            {
                MqttManager::publishMessage("Robots/" + MqttManager::getMacAddress() + "/From/Status/BatteryChargePct", std::to_string(RobotManager::getBatteryState()));

                DynamicJsonDocument doc(1024);
                Coordinates currentPosition = RobotManager::getPosition();
                doc["x"] = currentPosition.x;
                doc["y"] = currentPosition.y;
                serializeJson(doc, mqttMessage);
                doc.clear();
                MqttManager::publishMessage("Robots/" + MqttManager::getMacAddress() + "/From/Status/CurrentPosition", mqttMessage);

                delay(500);
            }

            if (nextDeliveryStep.waypointType == WaypointType::WAYPOINT_DEPOSIT || nextDeliveryStep.waypointType == WaypointType::WAYPOINT_HANDOVER)
            {
                int index = 0;
                for (int i = 0; i < storageArray.size(); ++i)
                {
                    if (storageArray[i] == nextDeliveryStep.productId)
                    {
                        index = i;
                        break;
                    }
                }
                RobotManager::placePackage(index);
                while (RobotManager::getArmState() != RobotArmState::RobotArmStateFinished)
                {
                }

                storageArray[index] = "";
            }

            if (nextDeliveryStep.waypointType == WaypointType::WAYPOINT_PARK_DISTRIBUTION_CENTER)
            {
                int index = 0;
                for (int i = 0; i < storageArray.size(); ++i)
                {
                    if (storageArray[i].empty())
                    {
                        index = i;
                        break;
                    }
                }
                RobotManager::pickPackage(index);
                while (RobotManager::getArmState() != RobotArmState::RobotArmStateFinished)
                {
                }
                storageArray[index] = nextDeliveryStep.productId;
            }
        }
        MqttManager::publishMessage("Robots/" + MqttManager::getMacAddress() + "/From/Status/DeliveryDone", true);
        break;

    default:
        programState = ProgramState::ProgramStateError;
        break;
    }
}