#include <Arduino.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>

HardwareSerial mySerial(1);

enum RobotState
{
    RobotStateError,
    RobotStateBusy,
    RobotStateFinished
};

bool getRobotState(RobotState *state)
{
    DynamicJsonDocument sendDoc(1024);
    DynamicJsonDocument recieveDoc(1024);

    *state = RobotStateError;

    sendDoc["Command"] = "GetDrivingState";
    serializeJson(sendDoc, mySerial);
    mySerial.println();

    deserializeJson(recieveDoc, mySerial);

    String returnedstate = recieveDoc["Data"]["State"];

    if (returnedstate == "Finished")
    {
        *state = RobotState::RobotStateFinished;
    }
    else if (returnedstate == "Busy")
    {
        *state = RobotState::RobotStateBusy;
    }

    return true;
}

bool SetNextWaypoint(int x, int y)
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "SetNextWaypoint";
    sendDoc["Data"]["x"] = x;
    sendDoc["Data"]["y"] = y;

    serializeJson(sendDoc, mySerial);
    mySerial.println();

    return true;
}

bool GoToWaypoint(int x, int y)
{
    SetNextWaypoint(x, y);

    delay(100);
    RobotState currentRobotState;

    do
    {
        getRobotState(&currentRobotState);
    } while (currentRobotState != RobotState::RobotStateFinished);

    return true;
}

void setup()
{
    Serial.begin(115200);
    mySerial.begin(115200, SERIAL_8N1, 18, 17);

    delay(2000);
    Serial.println("Hello:");



GoToWaypoint(50,300);
delay(4000);

GoToWaypoint(150,300);
GoToWaypoint(150,200);
GoToWaypoint(50,200);
GoToWaypoint(150,200);
GoToWaypoint(250,150);
GoToWaypoint(250,50);
GoToWaypoint(250,150);






   
}

void loop()
{
    // mySerial.println("{\"Command\":\"GetDrivingState\"}");
    // while (mySerial.available())
    // {
    //     char receivedChar = mySerial.read();
    //     Serial.write(receivedChar);
    // }

    // Serial.println();

    // mySerial.println("{\"Command\":\"GetCurrentPosition\"}");
    // while (mySerial.available())
    // {
    //     char receivedChar = mySerial.read();
    //     Serial.write(receivedChar);
    // }

    // Serial.println();

    // mySerial.println("{\"Command\":\"GetBatteryState\"}");
    // while (mySerial.available())
    // {
    //     char receivedChar = mySerial.read();
    //     Serial.write(receivedChar);
    // }

    // Serial.println("--------------------------------------------");

    // delay(1000);
}

// #include <Arduino.h>

// #define LED_PIN 1

// void blink(void *pvParameter)
// {
//     while (true)
//     {
//         digitalWrite(LED_PIN, HIGH);
//         delay(500);
//         digitalWrite(LED_PIN, LOW);
//         delay(500);
//     }
// }

// bool isPrime(int n)
// {
//     if (n <= 1)
//     {
//         return false;
//     }

//     for (int i = 2; i <= n / 2; ++i)
//     {
//         if (n % i == 0)
//         {
//             return false;
//         }
//     }

//     return true;
// }

// void prime(void *pvParameter)
// {
//     while (true)
//     {
//         for (unsigned int i = 0; i < UINT_MAX; i++)
//         {
//             if (isPrime(i))
//             {
//                 Serial.println(i);
//             }
//         }
//     }
// }

// void setup()
// {
//     Serial.begin(115200);

//     xTaskCreate(&blink, "blink", 2048, NULL, 2, NULL);
//     xTaskCreate(&prime, "prime", 2048, NULL, 5, NULL);

//     pinMode(LED_PIN, OUTPUT);
// }

// void loop()
// {
// }

// #include <Arduino.h>
// #include "Audio.h"
// #include <Wire.h>
// #include <HardwareSerial.h>

// // I2S Connections
// #define I2S_DOUT 7
// #define I2S_BCLK 5
// #define I2S_LRC 6

// #define SDA_PIN 1
// #define SCL_PIN 2

// // Create Audio object
// Audio audio;

// HardwareSerial mySerial(1);

// void setup()
// {
//     SPIFFS.begin();

//     Serial.begin(115200);
//     mySerial.begin(115200, SERIAL_8N1, 18, 17);

//     delay(2000);

//     Wire.begin(SDA_PIN, SCL_PIN); // Initialize the I2C bus with the specified pins

//     while (!Serial)
//         ; // Wait for serial connection
//     Serial.println("I2C Address Check");

//     byte address = 0x28; // Replace with the desired address to check
//     Wire.beginTransmission(address);
//     byte error = Wire.endTransmission();

//     if (error == 0)
//     {
//         Serial.print("Device found at address 0x");
//         if (address < 16)
//         {
//             Serial.print("0");
//         }
//         Serial.println(address, HEX);
//     }
//     else
//     {
//         Serial.print("No device found at address 0x");
//         if (address < 16)
//         {
//             Serial.print("0");
//         }
//         Serial.println(address, HEX);
//     }

//     mySerial.println("Hello World");

//     // Setup I2S
//     audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

//     // Set Volume
//     audio.setVolume(0xFF);

//     // Open music file
//     audio.connecttoFS(SPIFFS, "/hallo.mp3");
// }

// void loop()
// {
//     audio.loop();

//     if (mySerial.available())
//     {
//         char receivedChar = mySerial.read();
//         Serial.write(receivedChar);
//     }
// }

// lib_deps = esphome/ESP32-audioI2S@^2.0.6

// HardwareSerial mySerial(1);

// void setup()
// {
//     Serial.begin(115200);
//     mySerial.begin(115200, SERIAL_8N1, 18, 17);

//     delay(2000);
//     Serial.println("Hello:");

//     String jsonString = ;
//     mySerial.println("{\"Command\":\"SetNextWaypoint\",\"Data\":{\"x\":\"450\",\"y\":\"200\"}}");

//     delay(500);
// }

// void loop()
// {
//     mySerial.println("{\"Command\":\"GetDrivingState\"}");
//     while (mySerial.available())
//     {
//         char receivedChar = mySerial.read();
//         Serial.write(receivedChar);
//     }

//     Serial.println();

//     mySerial.println("{\"Command\":\"GetCurrentPosition\"}");
//     while (mySerial.available())
//     {
//         char receivedChar = mySerial.read();
//         Serial.write(receivedChar);
//     }

//     Serial.println();

//     mySerial.println("{\"Command\":\"GetBatteryState\"}");
//     while (mySerial.available())
//     {
//         char receivedChar = mySerial.read();
//         Serial.write(receivedChar);
//     }

//     Serial.println("--------------------------------------------");

//     delay(1000);
// }