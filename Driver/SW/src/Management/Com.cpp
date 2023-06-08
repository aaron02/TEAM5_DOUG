/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Com.h"

OpcodeHandler PacketHandlers[Functions::MAXFUNCTIONS];

Communication::Communication(Navigation* mNav ,Greifer* mGrip, PDB* mPwr, Odometry* mOdo) : mNavigation(mNav), mGreifer(mGrip), mPower(mPwr), mOdometry(mOdo)
{
    // Nullify Everything
    for (auto& PacketHandler : PacketHandlers)
    {
        PacketHandler.handler = nullptr;
    }
    loadHandlers();
}

Communication::~Communication()
{

}

void Communication::loadHandlers()
{
    PacketHandlers[Functions::GetCurrentPosition].handler = &Communication::handleGetCurrentPosition;
    PacketHandlers[Functions::SetDrivingWaypoint].handler = &Communication::handleSetDrivingWaypoint;
    PacketHandlers[Functions::AbortDriving].handler = &Communication::handleAbortDriving;
    PacketHandlers[Functions::GetDrivingState].handler = &Communication::handleGetDrivingState;
    PacketHandlers[Functions::SetArmState].handler = &Communication::handleSetArmStatus;
    PacketHandlers[Functions::GetArmState].handler = &Communication::handleGetArmStatus;
    PacketHandlers[Functions::PickPackage].handler = &Communication::handlePickPackage;
    PacketHandlers[Functions::PlacePackage].handler = &Communication::handlePlacePackage;
    PacketHandlers[Functions::GetBatteryState].handler = &Communication::handleGetBatteryState;
    PacketHandlers[Functions::CustomerAccepted].handler = &Communication::handleCustomerAccepted;
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

        OpcodeHandler* handler = &PacketHandlers[functionIndex];
        // Valid Packet :>
        if (handler->handler == 0)
        {
            sLogger.debug("Received Unhandledcommand %s with Index %u", cCommandName.c_str(), functionIndex);
        }
        else
        {
            sLogger.debug("Received Command %s with Index %u", cCommandName.c_str(), functionIndex);
            (this->*handler->handler)(doc);
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

// Handlers
void Communication::handleGetCurrentPosition(JsonDocument& doc)
{
    if (mOdometry)
    {
        DynamicJsonDocument doc(1024);
        doc["Data"]["x"] = std::to_string(static_cast<int32_t>(mOdometry->GetPosition()->getX()));
        doc["Data"]["y"] = std::to_string(static_cast<int32_t>(mOdometry->GetPosition()->getY()));
        serializeJson(doc, Serial1);
    }
    else
    {
        response("Error");
    }
}

void Communication::handleSetDrivingWaypoint(JsonDocument& doc)
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
}

void Communication::handleAbortDriving(JsonDocument& doc)
{
    // Stop Movement
    if (mNavigation)
        mNavigation->abortDriving();

    response("OK");
}

void Communication::handleGetDrivingState(JsonDocument& doc)
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
}

void Communication::handleSetArmStatus(JsonDocument& doc)
{
    uint8_t armState = doc["Data"];
    mGreifer->setArmStatus(ArmStatus(armState));
}

void Communication::handleGetArmStatus(JsonDocument& doc)
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
}

void Communication::handlePickPackage(JsonDocument& doc)
{
    uint32_t lagerIndex = doc["Data"]["Lagerindex"];
    if (mGreifer)
    {
        mGreifer->setLagerIndex(lagerIndex);
        mGreifer->setArmStatus(AS_PickPackage);
    }

    if (debug)
        sLogger.debug("handlePickPackage to Index %u", lagerIndex);
}

void Communication::handlePlacePackage(JsonDocument& doc)
{
    uint32_t lagerIndex = doc["Data"]["Lagerindex"];
    bool autonom = doc["Data"]["Autonom"];
    if (mGreifer)
    {
        mGreifer->setLagerIndex(lagerIndex);
        mGreifer->setArmStatus(AS_PlacePackage);
        mGreifer->PaketKundeOderAblageort = autonom;
    }

    if (debug)
        sLogger.debug("handlePlacePackage from Index %u", lagerIndex);
}

void Communication::handleGetBatteryState(JsonDocument& doc)
{
    // Response with Battery in %
    if (mPower)
    {
        sLogger.debug("%u", static_cast<int32_t>(mPower->GetVoltagePct()));
        responseData("BatteryState", std::to_string(33));
        //responseData("BatteryState", std::to_string(static_cast<int32_t>(mPower->GetVoltagePct())));
    }
    else
    {
        responseData("BatteryState", "Error");
    }
}

void Communication::handleCustomerAccepted(JsonDocument& doc)
{
    if (mGreifer)
    {
        mGreifer->PaketAnnahmeBestätigungKunde = true;
    }

    if (debug)
        sLogger.debug("handleCustomerAccepted");
}