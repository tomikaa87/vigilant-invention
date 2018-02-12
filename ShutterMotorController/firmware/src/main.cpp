#include <Arduino.h>

#include "BlynkHandler.h"
#include "ButtonHandler.h"
#include "RelayController.h"
#include "TemperatureSensor.h"

BlynkHandler* g_blynkHandler = nullptr;
ButtonHandler* g_buttonHandler = nullptr;
TemperatureSensor* g_temperatureSensor = nullptr;
RelayController* g_relayController = nullptr;

#define ERROR_ADD_EVENT_HANDLER(__CLASS, __HANDLER_NAME) \
    Serial.println("Error: failed to add event handler of \"" __CLASS "\" to \"" __HANDLER_NAME "\"")

void setup()
{
    Serial.begin(74880);

    Serial.println("Setting up BlynkHandler");

    static BlynkHandler s_blynkHandler;
    g_blynkHandler = &s_blynkHandler;

    Serial.println("Setting up ButtonHandler");

    static ButtonHandler s_buttonHandler;
    g_buttonHandler = &s_buttonHandler;

    Serial.println("Setting up TemperatureSensor");

    static TemperatureSensor s_temperatureSensor;
    g_temperatureSensor = &s_temperatureSensor;

    Serial.println("Setting up RelayController");

    static RelayController s_relayController;
    g_relayController = &s_relayController;

    Serial.println("Registering event handlers");

    auto ok = s_temperatureSensor.temperatureChangedEvent().addHandler([](const int16_t value) {
        s_blynkHandler.updateTemperature(value);
    });

    if (!ok)
        ERROR_ADD_EVENT_HANDLER("TemperatureSensor", "temperatureChangedEvent");

    ok = s_blynkHandler.blynkEvent().addHandler([](const BlynkHandler::EventType event, const int param) {
        switch (event)
        {
            case BlynkHandler::EventType::RelayButtonPressed:
                s_relayController.pulse(param);
                break;
        }
    });

    if (!ok)
        ERROR_ADD_EVENT_HANDLER("BlynkHandler", "blynkEvent");

    auto registered = g_buttonHandler->buttonEvent().addHandler([](const ButtonHandler::ButtonEventType e, const ButtonHandler::Button b) {
        Serial.printf("Test button event handler: e = %d, b = %d\r\n", (int)e, (int)b);

        if (e == ButtonHandler::ButtonEventType::Pressed)
        {
            switch (b)
            {
                case ButtonHandler::Button::Button1:
                    s_relayController.switchOn(0);
                    break;

                case ButtonHandler::Button::Button2:
                    s_relayController.switchOff(0);
                    break;

                case ButtonHandler::Button::Button3:
                    s_relayController.toggle(0);
                    break;

                case ButtonHandler::Button::Button4:
                    s_relayController.pulse(0);
                    break;

                default:
                    break;
            }
        }
    });

    Serial.printf("Test button event handler registered: %s\r\n", (registered ? "Yes" : "No"));

    registered = g_temperatureSensor->temperatureChangedEvent().addHandler([](const int16_t temp) {
        Serial.printf("Test temperature changed event handler: temp = %d\r\n", temp);
    });

    Serial.printf("Test temperature changed event handler registered: %s\r\n", (registered ? "Yes" : "No"));

    Serial.println("Setup finished");
}

void loop()
{
    g_blynkHandler->task();
    g_buttonHandler->task();
    g_temperatureSensor->task();
    g_relayController->task();
}