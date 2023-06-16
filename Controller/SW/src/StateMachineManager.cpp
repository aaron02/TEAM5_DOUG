#include "StateMachineManager.h"

ProgramState StateMachineManager::currentProgramState = ProgramStateUndefined;
bool StateMachineManager::isFirstRun = true;

void StateMachineManager::changeState(ProgramState newState)
{
    // If the new state is the same as the current state, do nothing
    if (currentProgramState == newState)
    {
        return;
    }

    // Generate and print log message
    std::string logMessage = "Change ProgramState state to ";
    switch (newState)
    {
    case ProgramState::ProgramStateInit:
        logMessage += "Init";
        break;

    case ProgramState::ProgramStateRecieveOrder:
        logMessage += "Recieve order";
        break;

    case ProgramState::ProgramStateRecieveBackupOrder:
        logMessage += "Recieve backup order";
        break;

    case ProgramState::ProgramStateDeliverOrder:
        logMessage += "Deliver order";
        break;

    case ProgramState::ProgramStateFinished:
        logMessage += "Finisshed";
        break;

    case ProgramState::ProgramStateError:
        logMessage += "Error";
        break;

    default:
        break;
    }

    // Write the log message
    Log::println(LogType::LOG_TYPE_LOG, "StateMachineManager", logMessage);

    // Change state
    currentProgramState = newState;

    // Reset firstRun flag
    isFirstRun = true;
}

ProgramState StateMachineManager::getCurrentState()
{
    // Return current state
    return currentProgramState;
}

bool StateMachineManager::getIsFirstRun()
{
    // Return firstRun flag and set it false
    if (isFirstRun)
    {
        isFirstRun = false;
        return true;
    }
    return false;
}