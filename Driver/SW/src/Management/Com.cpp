/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Com.h"

Communication::Communication(Navigation* mNav ,Greifer* mGrip, PDB* mPwr, Odometry* mOdo) : mNavigation(mNav), mGreifer(mGrip), mPower(mPwr), mOdometry(mOdo)
{

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
                if (doc.size() < 2 || doc["Data"].size() < 2)
                {
                    response("Error", "ArgumentError");
                    return;
                }

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
                mGreifer->setArmStatus(ArmStatus(armState));
            } break;

            case GetArmState:
            {
                // Response with Arm State
                switch (mGreifer->getArmStatus())
                {
                    case ArmStatus::AS_Error:
                    {
                        response("Error");
                    } break;
                    case ArmStatus::AS_Ready:
                    {
                        response("Ready");
                    } break;
                    case ArmStatus::AS_Grundstellung:
                    {
                        response("Stored");
                    } break;
                    case ArmStatus::AS_PickPackage:
                    {
                        response("PickingPackage");
                    } break;
                    case ArmStatus::AS_PlacePackage:
                    {
                        response("PlacingPackage");
                    } break;
                    default:
                        response("Undefined");
                        break;
                }                
            } break;

            case PickPackage:
            {
                uint32_t lagerIndex = doc["Data"]["Lagerindex"];
                mGreifer->setLagerIndex(lagerIndex);
                mGreifer->setArmStatus(AS_PickPackage);
            } break;

            case PlacePackage:
            {
                uint32_t lagerIndex = doc["Data"]["Lagerindex"];
                mGreifer->setLagerIndex(lagerIndex);
                mGreifer->setArmStatus(AS_PlacePackage);
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
    std::unordered_map<std::string, Functions>::iterator itr = mOpcodes.find(command);

    if (itr != mOpcodes.end())
        return mOpcodes.find(command)->second;

    return Functions::Unk;
}

void Communication::response(std::string response, std::string message)
{
    DynamicJsonDocument doc(1024);
    doc["Response"] = response;
        if (message.size())
            doc["Message"] = message;

    serializeJson(doc, Serial1);
}

void Communication::responseData(std::string response, std::string message)
{
    DynamicJsonDocument doc(1024);
    doc["Data"][response] = message;
    serializeJson(doc, Serial1);
}