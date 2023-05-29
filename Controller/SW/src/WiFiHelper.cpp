#include "WiFiHelper.h"

WiFiHelper::WiFiHelper(String ssid, String password, LogHelper &logHelper, WiFiClient &wifiClient)
    : ssid(ssid), password(password), logHelper(logHelper), wifiClient(wifiClient)
{
}

bool WiFiHelper::connect(unsigned int connectionTimeout_ms)
{
    logHelper.println(LOG_LEVEL_LOG, "Connecting to Wifi (" + ssid + ") with timeout of " + String(connectionTimeout_ms) + "ms...");

    // Connect to Wifi
    WiFi.begin(ssid.c_str(), password.c_str());

    // Wait for connection with timeout
    unsigned long startMillis = millis();
    bool finished = false;
    while ((millis() - startMillis) < connectionTimeout_ms && WiFi.status() != WL_CONNECTED)
    {
    }

    // Check Connection
    if (WiFi.status() != WL_CONNECTED)
    {
        logHelper.println(LOG_LEVEL_ERROR, "Connection to Wifi failed");
        return false;
    }

    logHelper.println(LOG_LEVEL_LOG, "Connection to Wifi successful IP: " + WiFi.localIP().toString() + " MAC: " + WiFi.macAddress());

    return true;
}

String WiFiHelper::getMacAddress()
{
    return  WiFi.macAddress();
}
