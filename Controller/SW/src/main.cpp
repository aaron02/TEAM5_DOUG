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

// Backup order in case the mqtt server is not available
const std::string backupOrder = R"({"deliveryId":"27a88501-a89a-48ec-8f85-d97eb925920f","productsToPickUp":[{"productId":"0c6cc4aa-4ed2-472f-a299-3c31903947fd","storageLocation":1,"quantity":1},{"productId":"25535516-6bf1-420e-8184-38c8686ff554","storageLocation":2,"quantity":1},{"productId":"94b98de0-e02e-409f-bd76-cfb595933b60","storageLocation":4,"quantity":1},{"productId":"3ca5190a-834c-4fba-8f37-bf3f33daf52d","storageLocation":3,"quantity":1}],"deliverySteps":[{"id":1,"type":"parkPosition","authorizationKey":"","productId":"","plannedDeliveryTime":"2023-05-29T19:41:37.7570977+02:00","coordinates":{"x":0,"y":-800}},{"id":2,"type":"waypoint","authorizationKey":"","productId":"","plannedDeliveryTime":"2023-05-29T19:41:59.7570977+02:00","coordinates":{"x":1100,"y":-800}},{"id":3,"type":"waypoint","authorizationKey":"","productId":"","plannedDeliveryTime":"2023-05-29T19:42:15.7570977+02:00","coordinates":{"x":1100,"y":0}},{"id":4,"type":"distributionCenter","authorizationKey":"","productId":"","plannedDeliveryTime":"2023-05-29T19:44:37.7570977+02:00","coordinates":{"x":0,"y":0}},{"id":5,"type":"waypoint","authorizationKey":"","productId":"","plannedDeliveryTime":"2023-05-29T19:44:59.7570977+02:00","coordinates":{"x":1100,"y":0}},{"id":6,"type":"deposit","authorizationKey":"","productId":"0c6cc4aa-4ed2-472f-a299-3c31903947fd","plannedDeliveryTime":"2023-05-29T19:46:19.7570977+02:00","coordinates":{"x":1100,"y":1000}},{"id":7,"type":"waypoint","authorizationKey":"","productId":"","plannedDeliveryTime":"2023-05-29T19:46:39.7570977+02:00","coordinates":{"x":1100,"y":0}},{"id":8,"type":"waypoint","authorizationKey":"","productId":"","plannedDeliveryTime":"2023-05-29T19:46:57.7570977+02:00","coordinates":{"x":2000,"y":0}},{"id":9,"type":"handOver","authorizationKey":"test1","productId":"25535516-6bf1-420e-8184-38c8686ff554","plannedDeliveryTime":"2023-05-29T19:48:17.7570977+02:00","coordinates":{"x":2000,"y":1000}},{"id":10,"type":"waypoint","authorizationKey":"","productId":"","plannedDeliveryTime":"2023-05-29T19:48:37.7570977+02:00","coordinates":{"x":2000,"y":0}},{"id":11,"type":"waypoint","authorizationKey":"","productId":"","plannedDeliveryTime":"2023-05-29T19:48:55.7570977+02:00","coordinates":{"x":2900,"y":0}},{"id":12,"type":"deposit","authorizationKey":"","productId":"3ca5190a-834c-4fba-8f37-bf3f33daf52d","plannedDeliveryTime":"2023-05-29T19:50:15.7570977+02:00","coordinates":{"x":2900,"y":1000}},{"id":13,"type":"waypoint","authorizationKey":"","productId":"","plannedDeliveryTime":"2023-05-29T19:50:35.7570977+02:00","coordinates":{"x":2900,"y":0}},{"id":14,"type":"handOver","authorizationKey":"test2","productId":"94b98de0-e02e-409f-bd76-cfb595933b60","plannedDeliveryTime":"2023-05-29T19:51:57.7570977+02:00","coordinates":{"x":4000,"y":0}},{"id":15,"type":"waypoint","authorizationKey":"","productId":"","plannedDeliveryTime":"2023-05-29T19:52:19.7570977+02:00","coordinates":{"x":2900,"y":0}}]})";

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
                StateMachineManager::changeState(ProgramStateError);
                break;
            }

            // Initialize the driver
            if (!RobotManager::initialize(ROBOT_BAUD_RATE, ROBOT_TIMEOUT))
            {
                StateMachineManager::changeState(ProgramStateError);
                break;
            }
            // Initialize the MQTT manager
            if (!MqttManager::initialize(WIFI_SSID, WIFI_PASSWORD, WIFI_TIMEOUT, MQTT_IP, MQTT_PORT, MQTT_ID, MQTT_MAX_BUFFER_SIZE, MQTT_TIMEOUT))
            {
                StateMachineManager::changeState(ProgramStateError);
                break;
            }
        }

        // Unsuscribe from all topics
        MqttManager::unsubscribeAllTopics();

        // Change the program state to the next state
        StateMachineManager::changeState(ProgramStateRecieveOrder);
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
            StateMachineManager::changeState(ProgramStateDeliverOrder);
        }
        else
        {
            // Write log message
            Log::println(LogType::LOG_TYPE_LOG, "Loop", "No order recieved from the server using the backup order");

            // Execute a fixed order
            OrderManager::parse(backupOrder);

            // Change the program state to the next state
            StateMachineManager::changeState(ProgramStateDeliverOrder);
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
            StateMachineManager::changeState(ProgramStateFinished);
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
        StateMachineManager::changeState(ProgramStateError);
        break;
    }

    // Delay to prevent the state machine from running to fast
    delay(2000);
}