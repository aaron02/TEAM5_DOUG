#pragma once

#include <string>
#include <Arduino.h>
#include <Audio.h>

#include "Log.h"

/// @brief The different audio messages that can be played
enum AudioMessage
{
    AUDIO_MESSAGE_HOLD_CARD_TO_READER,
    AUDIO_MESSAGE_AUTHENTICATION_SUCCESSFUL,
    AUDIO_MESSAGE_AUTHENTICATION_FAILED,
    AUDIO_MESSAGE_DELIVERY_DONE_WITH_AUTHENTICATION,
    AUDIO_MESSAGE_DELIVERY_DONE_WITHOUT_AUTHENTICATION,
    AUDIO_MESSAGE_ROBOT_READY
};

class AudioManager
{
public:
    /// @brief Initialize the audio manager
    /// @return True if the initialization was successful otherwise false
    static bool initialize();

    /// @brief Play audio message (blocking)
    static void playMessage(AudioMessage audioMessage);

private:
    /// @brief The audio object
    static Audio audio;
};