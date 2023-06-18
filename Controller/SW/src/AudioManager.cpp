#include "AudioManager.h"

// Initialize static variables
Audio AudioManager::audio;

bool AudioManager::initialize()
{ // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "AudioManager", "Initializing SPIFFS...");
    
    // Initialize SPIFFS
    if (!SPIFFS.begin())
    {
        // Write log message
        Log::println(LogType::LOG_TYPE_ERROR, "AudioManager", "Initialization of SPIFFS failed");

        return false;
    }

    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "AudioManager", "Initialization of SPIFFS successful");

    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "AudioManager", "Initializing I2S...");

    // Setup I2S
    if (!audio.setPinout(5, 6, 7))
    {
        // Write log message
        Log::println(LogType::LOG_TYPE_ERROR, "AudioManager", "Initialization of I2S failed");

        return false;
    }

    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "AudioManager", "Initialization of I2S successful");

    // Set Volume
    audio.setVolume(0xFF);

    return true;
}

void AudioManager::playMessage(AudioMessage audioMessage)
{ // Stop playing
    audio.stopSong();

    // Get path of file to play
    std::string filePath;
    switch (audioMessage)
    {
    case AUDIO_MESSAGE_HOLD_CARD_TO_READER:
        filePath = "/hold_card_to_reader.mp3";
        break;

    case AUDIO_MESSAGE_AUTHENTICATION_SUCCESSFUL:
        filePath = "/authentication_successful.mp3";
        break;

    case AUDIO_MESSAGE_AUTHENTICATION_FAILED:
        filePath = "/authentication_failed.mp3";
        break;

    case AUDIO_MESSAGE_DELIVERY_DONE_WITH_AUTHENTICATION:
        filePath = "/delivery_done_with_auth.mp3";
        break;

    case AUDIO_MESSAGE_DELIVERY_DONE_WITHOUT_AUTHENTICATION:
        filePath = "/delivery_done_without_auth.mp3";
        break;

    case AUDIO_MESSAGE_ROBOT_READY:
        filePath = "/robot_ready.mp3";
        break;

    default:
        // Write log message
        Log::println(LogType::LOG_TYPE_ERROR, "AudioManager", "Unknown audio message");
        return;
    }

    // Play file
    audio.connecttoFS(SPIFFS, filePath.c_str());

    // Wait for file to be played
    while (audio.isRunning())
    {
        audio.loop();
    }
}