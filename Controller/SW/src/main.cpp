#include <Arduino.h>

#define LED_PIN 1

void blink(void *pvParameter)
{
    while (true)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
        delay(500);
    }
}

bool isPrime(int n)
{
    if (n <= 1)
    {
        return false;
    }

    for (int i = 2; i <= n / 2; ++i)
    {
        if (n % i == 0)
        {
            return false;
        }
    }

    return true;
}

void prime(void *pvParameter)
{
    while (true)
    {
        for (unsigned int i = 0; i < UINT_MAX; i++)
        {
            if (isPrime(i))
            {
                Serial.println(i);
            }
        }
    }
}

void setup()
{
    Serial.begin(115200);

    xTaskCreate(&blink, "blink", 2048, NULL, 2, NULL);
    xTaskCreate(&prime, "prime", 2048, NULL, 5, NULL);

    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
}