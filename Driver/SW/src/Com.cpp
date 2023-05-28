/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Com.h"

Communication::Communication(Navigation* mNav ,Greifer* mGrip, PDB* mPwr, Odometry* mOdo) : mNavigation(mNav), mGreifer(mGrip), mPower(mPwr), mOdometry(mOdo)
{
    // Populate our Functions Map
    functionHandle.insert(std::pair<std::string, Functions>("GetCurrentPosition", GetCurrentPosition));
    functionHandle.insert(std::pair<std::string, Functions>("SetNextWaypoint", SetDrivingWaypoint));
    functionHandle.insert(std::pair<std::string, Functions>("AbortDriving", AbortDriving));
    functionHandle.insert(std::pair<std::string, Functions>("GetDrivingState", GetDrivingState));
    functionHandle.insert(std::pair<std::string, Functions>("SetArmState", SetArmState));
    functionHandle.insert(std::pair<std::string, Functions>("GetArmState", GetArmState));
    functionHandle.insert(std::pair<std::string, Functions>("PickPackage", PickPackage));
    functionHandle.insert(std::pair<std::string, Functions>("PlacePackage", PlacePackage));
    functionHandle.insert(std::pair<std::string, Functions>("GetBatteryState", GetBatteryState));
}

Communication::~Communication()
{

}

void Communication::Update(uint64_t difftime)
{
    if (Serial1.available())
    {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, Serial1);

        std::string cCommandName = doc["Command"];

        uint8_t functionIndex = getFunctionIndex(cCommandName);

        sLogger.debug("Received Command %s with Index %u", cCommandName.c_str(), functionIndex);

        switch(functionIndex)
        {
            case GetCurrentPosition:
            {
                if (mOdometry)
                {
                    DynamicJsonDocument doc(1024);
                    doc["Data"]["x"] = std::to_string(mOdometry->GetPosition()->getX());
                    doc["Data"]["y"] = std::to_string(mOdometry->GetPosition()->getY());
                    serializeJson(doc, Serial1);
                }
                else
                {
                    response("Error");
                }
            } break;
            case SetDrivingWaypoint:
            {
                int32_t x = doc["Data"]["x"];
                int32_t y = doc["Data"]["y"];

                if (mNavigation)
                    mNavigation->setSollPosition(x, y);

                response("OK");
            } break;

            case AbortDriving:
            {
                // Stop Movement
                if (mNavigation)
                    mNavigation->abortDriving();

                response("OK");
            } break;

            case GetDrivingState:
            {
                // Respond with Movement State
                if (mNavigation)
                {
                    switch (mNavigation->getDrivingState())
                    {
                        case DRIVE_STATE_BUSY:
                        {
                            response("Busy");
                        } break;

                        case DRIVE_STATE_FINISHED:
                        {
                            response("Finished");
                        } break;

                        case DRIVE_STATE_ERROR:
                        {
                            response("Error");
                        } break;
                    }
                }
                else
                    response("Error");
            } break;

            case SetArmState:
            {
                uint8_t armState = doc["Data"];
            } break;

            case GetArmState:
            {
                // Response with Arm State
                response("Ready");
                response("Stored");
            } break;

            case PickPackage:
            {
                uint32_t lagerIndex = doc["Data"]["Lagerindex"];
            } break;

            case PlacePackage:
            {
                uint32_t lagerIndex = doc["Data"]["Lagerindex"];
            } break;

            case GetBatteryState:
            {
                // Response with Battery in %
                if (mPower)
                {
                    responseData("BatteryState", std::to_string(mPower->GetVoltagePct()));
                }
                else
                {
                    responseData("BatteryState", "Error");
                }
            } break;

            default:
                break;
        }
    }
}

uint8_t Communication::getFunctionIndex(std::string command)
{
    sFunctions::iterator itr = functionHandle.find(command);

    if (itr != functionHandle.end())
        return functionHandle.find(command)->second;

    return Functions::Unk;
}

void Communication::response(std::string response, std::string message)
{
    DynamicJsonDocument doc(1024);
    doc["Response"] = response;
        if (message.size())
            doc["ErrorMessage"] = message;

    serializeJson(doc, Serial1);
}

void Communication::responseData(std::string response, std::string message)
{
    DynamicJsonDocument doc(1024);
    doc["Data"][response] = message;
    serializeJson(doc, Serial1);
}