#include <Arduino.h>
#include "WiFiHelper.h"
#include "MqttHelper.h"
#include <ArduinoJson.h>

const String IP = "91.121.93.94";
const String SSID = "McDonalds";
const String PASSWORD = "burgerking";
const String MQTT_DEVICE_ID = "Dough";
const int MQTT_PORT = 1883;

String macAddress;

WiFiClient wifiClient;
LogHelper logHelper(Serial, 115200);
WiFiHelper wifiHelper(SSID, PASSWORD, logHelper, wifiClient);
MqttHelper mqttHelper(IP, MQTT_PORT, MQTT_DEVICE_ID, logHelper, wifiClient,10000);

void setup()
{
    logHelper.connect();
    wifiHelper.connect(100000);
    macAddress = wifiHelper.getMacAddress();
    mqttHelper.connect();
    mqttHelper.subscribe("Robots/"+macAddress+"/To/DeliveryOrder");
}

void loop()
{
    mqttHelper.loop();

    if (mqttHelper.hasMessage())
    {
        MqttMessage message = mqttHelper.getNextMessage();


    }
}