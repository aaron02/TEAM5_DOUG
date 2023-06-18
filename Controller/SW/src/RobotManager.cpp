#include "RobotManager.h"

// Initialize static variables
int RobotManager::currentBatteryState = -1;
Coordinates RobotManager::currentPosition;
RobotDrivingState RobotManager::drivingState = RobotDrivingState::RobotDrivingStateUndefined;
RobotArmState RobotManager::armState = RobotArmState::RobotArmStateUndefined;

bool RobotManager::initialize(unsigned long baudRate, unsigned long serialTimeout_ms)
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Initializing robot connection with timeout of " + std::to_string(serialTimeout_ms) + "ms");

    // Set arm state to busy
    armState = RobotArmState::RobotArmStateBusy;

    // Initialize the serial connection
    Serial1.begin(baudRate, SERIAL_8N1, 18, 17);

    // Wait for the serial connection to be established with a timeout of 2 seconds
    unsigned long startTime = millis();
    while (!Serial1 && millis() - startTime < serialTimeout_ms)
    {
    }

    // Check if the serial connection is established
    if (!Serial1)
    {
        // Write log message
        Log::println(LogType::LOG_TYPE_ERROR, "RobotManager", "Serial connection could not be established");

        // Return false if the serial connection is not established
        return false;
    }

    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Robot connection established");

    // Wait until arm state is ready
    while (armState != RobotArmState::RobotArmStateReady)
    {
        // Request the arm state
        requestArmState();

        // Wait for 1s to prevent the serial connection from being flooded
        delay(1000);

        // Prosess incoming messages
        processIncomingeMessages();
    }

    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Robot arm is ready");

    return true;
}

void RobotManager::startDrivingToWaypoint(Coordinates coordinates)
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Start driving to waypoint (" + std::to_string(coordinates.x) + ", " + std::to_string(coordinates.y) + ")");

    // Set the driving state to busy
    drivingState = RobotDrivingState::RobotDrivingStateBusy;

    // Create the JSON command
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["Command"] = "SetNextWaypoint";
    jsonDoc["Data"]["x"] = coordinates.x;
    jsonDoc["Data"]["y"] = coordinates.y;

    // Send the command to the robot
    sendCommand(jsonDoc);
}

void RobotManager::stopDriving()
{
    // Wirte log message
    Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Stop driving");

    // Create the JSON command
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["Command"] = "AbortDriving";

    // Send the command to the robot
    sendCommand(jsonDoc);
}

void RobotManager::startPickPackage(int robotStorageIndex, int distributionCenterIndex)
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Start picking package from distribution center " + std::to_string(distributionCenterIndex) + " to robot storage " + std::to_string(robotStorageIndex));

    // Set the arm state to busy
    armState = RobotArmState::RobotArmStateBusy;

    // Create the JSON command
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["Command"] = "PickPackage";
    jsonDoc["Data"]["RoboterIndex"] = robotStorageIndex;
    jsonDoc["Data"]["LagerIndex"] = distributionCenterIndex;

    // Send the command to the robot
    sendCommand(jsonDoc);
}

void RobotManager::startPlacePackage(int robotStorageIndex)
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Start placing package from robot storage " + std::to_string(robotStorageIndex));

    // Set the arm state to busy
    armState = RobotArmState::RobotArmStateBusy;

    // Create the JSON command
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["Command"] = "PlacePackage";
    jsonDoc["Data"]["RoboterIndex"] = robotStorageIndex;
    jsonDoc["Data"]["Autonom"] = 1;

    // Send the command to the robot
    sendCommand(jsonDoc);
}

void RobotManager::processIncomingeMessages()
{
    // Process all incoming messages until the serial connection is empty
    while (Serial1.available())
    {
        // Deserialize the incoming message into a JSON document
        DynamicJsonDocument jsonDoc(1024);
        deserializeJson(jsonDoc, Serial1);

        // Write log message
        std::string message;
        serializeJson(jsonDoc, message);
        Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Received message: " + message);

        // Check if the message contains the battery state
        if (jsonDoc["Command"] == "SendBatteryState")
        {
            // Write log message
            Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Battery state changed to " + std::to_string(jsonDoc["Data"]["BatteryState"].as<int>()));

            // Update the battery state
            currentBatteryState = jsonDoc["Data"]["BatteryState"].as<int>();
        }

        // Check if the message contains the current position
        if (jsonDoc["Command"] == "SendCurrentPosition")
        {
            // Write log message
            Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Current position changed to (" + std::to_string(jsonDoc["Data"]["x"].as<int>()) + ", " + std::to_string(jsonDoc["Data"]["y"].as<int>()) + ")");

            // Update the current position
            currentPosition.x = jsonDoc["Data"]["x"].as<int>();
            currentPosition.y = jsonDoc["Data"]["y"].as<int>();
        }

        // Check if the message contains the driving state
        if (jsonDoc["Command"] == "SendDrivingState")
        {
            // Update the driving state
            int stateInt = jsonDoc["Data"]["State"].as<int>();

            switch (stateInt)
            {
            case 0:
                // Write log message
                Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Driving state changed to error");

                drivingState = RobotDrivingState::RobotDrivingStateError;
                break;
            case 1:
                // Write log message
                Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Driving state changed to busy");

                drivingState = RobotDrivingState::RobotDrivingStateBusy;
                break;
            case 2:
                // Write log message
                Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Driving state changed to ready");

                drivingState = RobotDrivingState::RobotDrivingStateReady;
                break;
            default:
                break;
            }
        }

        // Check if the message contains the arm state
        if (jsonDoc["Command"] == "SendArmState")
        {
            // Update the arm state
            int stateInt = jsonDoc["Data"]["State"].as<int>();

            switch (stateInt)
            {
            case 0:
                // Write log message
                Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Arm state changed to busy");

                armState = RobotArmState::RobotArmStateBusy;
                break;
            case 1:
                // Write log message
                Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Arm state changed to ready");

                armState = RobotArmState::RobotArmStateReady;
                break;
            case 2:
                // Write log message
                Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Arm state changed to error");

                armState = RobotArmState::RobotArmStateBusy;
                break;
            case 3:
                // Write log message
                Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Arm state changed to busy");

                armState = RobotArmState::RobotArmStateBusy;
                break;
            case 4:
                // Write log message
                Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Arm state changed to ready");

                armState = RobotArmState::RobotArmStateReady;
                break;
            case 5:
                // Write log message
                Log::println(LogType::LOG_TYPE_LOG, "RobotManager", "Arm state changed to error");

                armState = RobotArmState::RobotArmStateError;
                break;
            default:
                break;
            }
        }
    }
}

void RobotManager::clearIncomingeMessages()
{
    // Check if there is data available on the serial connection
    while (Serial1.available())
    {
        // Read the data into a dummy variable
        char c = Serial1.read();
    }
}

int RobotManager::getBatteryState()
{
    return currentBatteryState;
}

Coordinates RobotManager::getCurrentPosition()
{
    return currentPosition;
}

RobotDrivingState RobotManager::getDrivingState()
{
    return drivingState;
}

RobotArmState RobotManager::getArmState()
{
    return armState;
}

void RobotManager::sendCommand(DynamicJsonDocument &payload)
{
    // Serialize the JSON document and send it to the robot
    serializeJson(payload, Serial1);
}

void RobotManager::requestArmState()
{
    // Create the JSON command
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["Command"] = "GetArmState";

    // Send the command to the robot
    sendCommand(jsonDoc);
}