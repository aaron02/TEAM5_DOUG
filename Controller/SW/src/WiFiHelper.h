#pragma once

#include <WiFi.h>
#include "LogHelper.h"

class WiFiHelper
{
public:
    WiFiHelper(String ssid, String password, LogHelper &logHelper, WiFiClient &wifiClient);
    bool connect(unsigned int connectionTimeout_ms);
    String getMacAddress();

private:
    WiFiClient wifiClient;
    String ssid;
    String password;
    LogHelper logHelper;
};