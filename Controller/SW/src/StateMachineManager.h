#pragma once

#include "Log.h"

/// @brief The different states of the state machine
enum ProgramState
{
    ProgramStateUndefined,
    ProgramStateInit,
    ProgramStateRecieveOrder,
    ProgramStateRecieveBackupOrder,
    ProgramStateDeliverOrder,
    ProgramStateFinished,
    ProgramStateError
};

/// @brief This class is used to manage the state machine
class StateMachineManager
{
public:
    /// @brief Change the current state
    /// @param newState The new state
    static void changeState(ProgramState newState);

    /// @brief Get the current state
    static ProgramState getCurrentState();

    /// @brief Get the first run flag and set it false
    static bool getIsFirstRun();

private:
    /// @brief The current state
    static ProgramState currentProgramState;

    /// @brief Flag to indicate if it ist the first run of the current state
    static bool isFirstRun;
};