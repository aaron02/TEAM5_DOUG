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

    // Reference Us in other Classes
    mNav->setCommunication(this);
    mGreifer->setCommunication(this);
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

        if (debug)
            sLogger.debug("Received Command %s with Index %u", cCommandName.c_str(), functionIndex);

        OpcodeHandler* handler = &PacketHandlers[functionIndex];
        // Valid Handler
        if (handler->handler == 0)
        {
            if (debug)
                sLogger.debug("Received Unhandledcommand %s with Index %u", cCommandName.c_str(), functionIndex);
        }
        else
        {
            if (debug)
                sLogger.debug("Received Command %s with Index %u", cCommandName.c_str(), functionIndex);
            (this->*handler->handler)(doc);
        }            
    }

    // Periodicaly Send Status Updates
    if (timer <= 0)
    {
        timer = 1 * TimeVar::Seconds;
        sendBatteryState();
        sendCurrentPosition();
    }
    else
        timer -= difftime;
}

uint8_t Communication::getFunctionIndex(std::string command)
{
    std::unordered_map<std::string, Functions>::iterator itr = mOpcodes.find(command);

    if (itr != mOpcodes.end())
        return mOpcodes.find(command)->second;

    return Functions::Unk;
}

// Handlers
void Communication::handleGetCurrentPosition(JsonDocument& doc)
{
    if (debug)
        sLogger.debug("handleGetCurrentPosition");

    sendCurrentPosition();
}

void Communication::handleSetDrivingWaypoint(JsonDocument& doc)
{
    if (debug)
        sLogger.debug("handleSetDrivingWaypoint");

    if (doc.size() < 2 || doc["Data"].size() < 2)
        return;

    int32_t x = doc["Data"]["x"];
    int32_t y = doc["Data"]["y"];

    if (mNavigation)
        mNavigation->setSollPosition(x, y);
}

void Communication::handleAbortDriving(JsonDocument& doc)
{
    // Stop Movement
    if (mNavigation)
        mNavigation->abortDriving();
}

void Communication::handleGetDrivingState(JsonDocument& doc)
{
    if (debug)
        sLogger.debug("handleGetDrivingState");

    sendDrivingState();
}

void Communication::handleSetArmStatus(JsonDocument& doc)
{
    uint8_t armState = doc["Data"];

    if (debug)
        sLogger.debug("handleSetArmStatus to armState = %u", armState);

    mGreifer->setArmStatus(ArmStatus(armState));
}

void Communication::handleGetArmStatus(JsonDocument& doc)
{
    if (debug)
        sLogger.debug("handleGetArmStatus");

    sendArmState();
}

void Communication::handlePickPackage(JsonDocument& doc)
{
    uint32_t roboterIndex = doc["Data"]["RoboterIndex"];
    uint32_t lagerIndex = doc["Data"]["LagerIndex"];
    
    if (debug)
        sLogger.debug("handlePickPackage to Index %u", lagerIndex);

    if (mGreifer)
    {
        mGreifer->setLagerIndex(lagerIndex);
        mGreifer->setArmStatus(AS_PickPackage);
    }
}

void Communication::handlePlacePackage(JsonDocument& doc)
{
    uint32_t lagerIndex = doc["Data"]["LagerIndex"];
    bool autonom = doc["Data"]["Autonom"];

    if (debug)
        sLogger.debug("handlePlacePackage from Index %u", lagerIndex);

    if (mGreifer)
    {
        mGreifer->setLagerIndex(lagerIndex);
        mGreifer->setArmStatus(AS_PlacePackage);
        mGreifer->PaketKundeOderAblageort = autonom;
    }
}

void Communication::handleGetBatteryState(JsonDocument& doc)
{
    if (debug)
        sLogger.debug("handleGetBatteryState");

    sendBatteryState();
}

void Communication::handleCustomerAccepted(JsonDocument& doc)
{
    if (debug)
        sLogger.debug("handleCustomerAccepted");

    if (mGreifer)
    {
        mGreifer->PaketAnnahmeBestätigungKunde = true;
    }
}

    void Communication::sendBatteryState()
    {
        int32_t voltagePct = static_cast<int32_t>(mPower->GetVoltagePct());
        if (debug)
            sLogger.debug("sendBatteryState Voltage Pct = %u", voltagePct);
        

        DynamicJsonDocument doc(1024);
        doc["Command"] = "SendBatteryState";
        doc["Data"]["BatteryState"] = voltagePct;
        serializeJson(doc, Serial1);
    }

    void Communication::sendCurrentPosition()
    {
        int32_t x = static_cast<int32_t>(mOdometry->GetPosition()->getX());
        int32_t y = static_cast<int32_t>(mOdometry->GetPosition()->getY());
        if (debug)
            sLogger.debug("sendCurrentPosition x = %i y = %i", x, y);

        DynamicJsonDocument doc(1024);
        doc["Command"] = "SendCurrentPosition";
        doc["Data"]["x"] = x;
        doc["Data"]["y"] = y;
        serializeJson(doc, Serial1);
    }

    void Communication::sendDrivingState()
    {
        uint8_t drivingState = mNavigation->getDrivingState();
        if (debug)
            sLogger.debug("sendDrivingState State = %u", drivingState);

        DynamicJsonDocument doc(1024);
        doc["Command"] = "SendDrivingState";
        doc["Data"]["State"] = drivingState;
        serializeJson(doc, Serial1);
    }

    void Communication::sendArmState()
    {
        uint8_t armState = mGreifer->getArmStatus();
        if (debug)
            sLogger.debug("sendArmState State = %u", armState);

        DynamicJsonDocument doc(1024);
        doc["Command"] = "SendArmState";
        doc["Data"]["State"] = armState;
        serializeJson(doc, Serial1);
    }