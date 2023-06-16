#include <Arduino.h>
#include <MqttManager.h>
#include <ArduinoJson.h>
#include <array>
#include <HardwareSerial.h>

#include "RobotManager.h"
#include "Log.h"
#include "OrderManager.h"
#include "Coordinates.h"
#include "Product.h"
#include "Log.h"
#include "MqttManager.h"
#include "StateMachineManager.h"
#include "BackupOrder.h"

// Log configuration
const unsigned long LOG_BAUD_RATE = 115200;
const unsigned long LOG_TIMEOUT = 10000;

// Robot configuration
const unsigned long ROBOT_BAUD_RATE = 115200;
const unsigned long ROBOT_TIMEOUT = 10000;

// WiFi credentials
// const std::string WIFI_SSID = "High5Dynamics";
// const std::string WIFI_PASSWORD = "H1gh5Dyn4m1cs!";
const std::string WIFI_SSID = "McDonalds";
const std::string WIFI_PASSWORD = "burgerking";
const unsigned long WIFI_TIMEOUT = 10000;

// MQTT configuration
// const IPAddress MQTT_IP(10, 1, 0, 1);
const IPAddress MQTT_IP(91, 121, 93, 94);
const std::string MQTT_ID = "Dough";
const int MQTT_PORT = 1883;
const uint16_t MQTT_MAX_BUFFER_SIZE = 10000;
const unsigned long MQTT_TIMEOUT = 10000;

void setup()
{
    // Initialize the state machine
    StateMachineManager::changeState(ProgramState::ProgramStateInit);
}

void loop()
{
    // Execute all the stuff to keep the MQTT connection alive
    MqttManager::keepAlive;

    // Process incoming messages from the robot
    RobotManager::processIncomingeMessages();

    // State machine
    switch (StateMachineManager::getCurrentState())
    {
    case ProgramState::ProgramStateInit:
        if (StateMachineManager::getIsFirstRun())
        {
            // Initialize the log
            if (!Log::initialize(LOG_BAUD_RATE, LOG_TIMEOUT))
            {
                StateMachineManager::changeState(ProgramState::ProgramStateError);
                break;
            }

            // Initialize the driver
            if (!RobotManager::initialize(ROBOT_BAUD_RATE, ROBOT_TIMEOUT))
            {
                StateMachineManager::changeState(ProgramState::ProgramStateError);
                break;
            }

            // Initialize the MQTT manager
            if (!MqttManager::initialize(WIFI_SSID, WIFI_PASSWORD, WIFI_TIMEOUT, MQTT_IP, MQTT_PORT, MQTT_ID, MQTT_MAX_BUFFER_SIZE, MQTT_TIMEOUT))
            {
                StateMachineManager::changeState(ProgramState::ProgramStateError);
                break;
            }
        }

        // Unsuscribe from all topics
        MqttManager::unsubscribeAllTopics();

        // Change the program state to the next state
        StateMachineManager::changeState(ProgramState::ProgramStateRecieveOrder);
        break;

    case ProgramState::ProgramStateRecieveOrder:
        if (StateMachineManager::getIsFirstRun())
        {
            // Request a new order
            MqttManager::requestOrder();
        }

        // Temporary delay to test the backup order
        delay(5000);

        // Check if there is a new order
        if (MqttManager::hasOrder())
        {
            // Change the program state to the next state
            StateMachineManager::changeState(ProgramState::ProgramStateDeliverOrder);
        }
        else
        {
            // Write log message
            Log::println(LogType::LOG_TYPE_LOG, "Loop", "No order recieved from the server using the backup order");

            // Change the program state to the next state
            StateMachineManager::changeState(ProgramState::ProgramStateRecieveBackupOrder);
        }
        break;

    case ProgramState::ProgramStateRecieveBackupOrder:
        if (StateMachineManager::getIsFirstRun())
        {
            // Write log message
            Log::println(LogType::LOG_TYPE_LOG, "Loop", "Parse backup order");

            // Parse the backup order
            OrderManager::parse(backupOrder);

            // Change the program state to the next state
            StateMachineManager::changeState(ProgramState::ProgramStateDeliverOrder);
        }
        break;

    case ProgramState::ProgramStateDeliverOrder:
        if (StateMachineManager::getIsFirstRun())
        {
            // Send update to the server
            MqttManager::sendCurrentDeliveryId(OrderManager::getdeliveryId());
            MqttManager::sendCurrentBatteryState(RobotManager::getBatteryState());
            MqttManager::sendCurrentPosition(RobotManager::getCurrentPosition());
        }

        // Ceck if the current delivery step has not started yet
        if (OrderManager::getCurrentDeliveryStep().state == DeliveryStepState::DELIVERY_STEP_STATE_NOT_STARTED)
        {
            // Set the current delivery step to driving
            OrderManager::getCurrentDeliveryStep().state = DeliveryStepState::DELIVERY_STEP_STATE_DRIVING;

            // Send update to the server
            MqttManager::sendCurrentDeliveryStep(OrderManager::getdeliveryId(), OrderManager::getCurrentDeliveryStep().id);

            // Set the driving waypoint
            RobotManager::startDrivingToWaypoint(OrderManager::getCurrentDeliveryStep().coordinates);
        }

        // Check if the current delivery step is driving
        if (OrderManager::getCurrentDeliveryStep().state == DeliveryStepState::DELIVERY_STEP_STATE_DRIVING)
        {
            // Check if the robot has arrived at the waypoint
            if (RobotManager::getDrivingState() == RobotDrivingState::RobotDrivingStateReady)
            {
                // Check if the waypoint was of the type waypoint or park position
                if (OrderManager::getCurrentDeliveryStep().waypointType == WaypointType::WAYPOINT || OrderManager::getCurrentDeliveryStep().waypointType == WaypointType::WAYPOINT_PARK_POSITION)
                {
                    // write Log message
                    Log::println(LogType::LOG_TYPE_LOG, "Loop", "Arrived at waypoint");

                    // Set the current delivery step to finished
                    OrderManager::getCurrentDeliveryStep().state = DeliveryStepState::DELIVERY_STEP_STATE_FINISHED;
                }

                // Check if the waypoint was of the type park distribution center
                else if (OrderManager::getCurrentDeliveryStep().waypointType == WaypointType::WAYPOINT_PARK_DISTRIBUTION_CENTER)
                {
                    // Write log message
                    Log::println(LogType::LOG_TYPE_LOG, "Loop", "Arrived at distribution center");

                    // Set the current delivery step to picking
                    OrderManager::getCurrentDeliveryStep().state = DeliveryStepState::DELIVERY_STEP_STATE_PICKING;

                    // Start picking the product
                    RobotManager::startPickPackage(OrderManager::getNextProductToPickUp().storageLocationRobot, OrderManager::getNextProductToPickUp().warehouseLocation);
                }

                // Check if the waypoint was of the type deposit or handover
                else if (OrderManager::getCurrentDeliveryStep().waypointType == WaypointType::WAYPOINT_DEPOSIT || OrderManager::getCurrentDeliveryStep().waypointType == WaypointType::WAYPOINT_HANDOVER)
                {
                    // Write log message
                    Log::println(LogType::LOG_TYPE_LOG, "Loop", "Arrived at deposit or handover");

                    // Set the current delivery step to placing
                    OrderManager::getCurrentDeliveryStep().state = DeliveryStepState::DELIVERY_STEP_STATE_PLACING;

                    // Start placing the product
                    RobotManager::startPlacePackage(OrderManager::getStorageLocationRobotByProductId(OrderManager::getCurrentDeliveryStep().productIdToPlace));
                }
            }

            // Send update to the server
            MqttManager::sendCurrentPosition(RobotManager::getCurrentPosition());
        }

        // Check if the current delivery step is picking
        if (OrderManager::getCurrentDeliveryStep().state == DeliveryStepState::DELIVERY_STEP_STATE_PICKING)
        {
            // Check the picking state
            if (RobotManager::getArmState() == RobotArmState::RobotArmStateReady)
            {
                // Set the current product to picked up
                OrderManager::getNextProductToPickUp().state = ProductState::PRODUCT_STATE_PICKED_UP;

                // Check if there are products left to pick up
                if (OrderManager::hasProductToPickUp())
                {
                    // Start picking the next product
                    RobotManager::startPickPackage(OrderManager::getNextProductToPickUp().storageLocationRobot, OrderManager::getNextProductToPickUp().warehouseLocation);
                }
                else
                {
                    // Set the current delivery step to finished
                    OrderManager::getCurrentDeliveryStep().state = DeliveryStepState::DELIVERY_STEP_STATE_FINISHED;

                    // Send update to the server
                    MqttManager::sendCurrentPosition(RobotManager::getCurrentPosition());
                }
            }
        }

        // Check if the current delivery step is placing
        if (OrderManager::getCurrentDeliveryStep().state == DeliveryStepState::DELIVERY_STEP_STATE_PLACING)
        {
            // Check the placing state
            if (RobotManager::getArmState() == RobotArmState::RobotArmStateReady)
            {
                // Set the current delivery step to finished
                OrderManager::getCurrentDeliveryStep().state = DeliveryStepState::DELIVERY_STEP_STATE_FINISHED;

                // Send update to the server
                MqttManager::sendCurrentPosition(RobotManager::getCurrentPosition());
            }
        }

        // Check if the current order is finished
        if (!OrderManager::hasDeliveryStepLeft())
        {
            // Send update to the server
            MqttManager::sendDeliveryDone();

            // Change the program state to the next state
            StateMachineManager::changeState(ProgramState::ProgramStateFinished);
        }

        break;

    case ProgramState::ProgramStateFinished:
        // Write log message
        Log::println(LogType::LOG_TYPE_LOG, "Loop", "Finished!!");

        break;

    case ProgramState::ProgramStateError:
        // Write log message
        Log::println(LogType::LOG_TYPE_LOG, "Loop", "ERROR!!");
        break;

    default:
        StateMachineManager::changeState(ProgramState::ProgramStateError);
        break;
    }

    // Delay to prevent the state machine from running to fast
    delay(2000);
}