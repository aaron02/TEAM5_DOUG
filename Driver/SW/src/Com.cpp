/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Com.h"

Communication::Communication()
{
    // Populate our Functions Map
    functionHandle.insert(std::pair<std::string, Functions>("GetCurrentPosition", GetCurrentPosition));
    functionHandle.insert(std::pair<std::string, Functions>("SetDrivingWaypoint", SetDrivingWaypoint));
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
            case SetDrivingWaypoint:
            {
                uint32_t x = doc["Data"][0];
                uint32_t y = doc["Data"][1];

                sLogger.debug("Data:: X=%u Y=%u", x, y);
            } break;
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