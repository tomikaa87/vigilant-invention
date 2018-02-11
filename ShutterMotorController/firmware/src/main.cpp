#include <Arduino.h>

#include "ButtonHandler.h"
#include "TemperatureSensor.h"

ButtonHandler* g_buttonHandler = nullptr;
TemperatureSensor* g_temperatureSensor = nullptr;

void setup()
{
    Serial.begin(115200);

    static ButtonHandler s_buttonHandler;
    g_buttonHandler = &s_buttonHandler;

    static TemperatureSensor s_temperatureSensor;
    g_temperatureSensor = &s_temperatureSensor;

    auto registered = g_buttonHandler->buttonEvent().addHandler([](const ButtonHandler::ButtonEventType e, const ButtonHandler::Button b) {
        Serial.printf("Test button event handler: e = %d, b = %d\r\n", (int)e, (int)b);
    });

    Serial.printf("Test button event handler registered: %s\r\n", (registered ? "Yes" : "No"));

    registered = g_temperatureSensor->temperatureChangedEvent().addHandler([](const int16_t temp) {
        Serial.printf("Test temperature changed event handler: temp = %d\r\n", temp);
    });

    Serial.printf("Test temperature changed event handler registered: %s\r\n", (registered ? "Yes" : "No"));
}

void loop()
{
    g_buttonHandler->task();
    g_temperatureSensor->task();
}