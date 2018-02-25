#include <Arduino.h>

#include "Automator.h"
#include "BlynkHandler.h"
#include "ButtonHandler.h"
#include "Clock.h"
#include "PersistentStorage.h"
#include "RelayController.h"
#include "TemperatureSensor.h"

static void(*runTasks)() = nullptr;

#define ERROR_ADD_EVENT_HANDLER(__CLASS, __HANDLER_NAME) \
    Serial.println("Error: failed to add event handler of \"" __CLASS "\" to \"" __HANDLER_NAME "\"")

void setup()
{
    Serial.begin(74880);

    Serial.println("Setting up PersistentStorage");
    static PersistentStorage s_persistentStorage;
    s_persistentStorage.loadConfiguration();

    Serial.println("Setting up BlynkHandler");
    static BlynkHandler s_blynkHandler{ s_persistentStorage };

    Serial.println("Setting up ButtonHandler");
    static ButtonHandler s_buttonHandler;

    Serial.println("Setting up TemperatureSensor");
    static TemperatureSensor s_temperatureSensor;

    Serial.println("Setting up RelayController");
    static RelayController s_relayController;

    Serial.println("Setting up Clock");
    static Clock s_clock;

    Serial.println("Setting up Automator");
    static Automator s_automator{ s_blynkHandler, s_clock, s_persistentStorage, s_relayController };

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

    auto registered = s_buttonHandler.buttonEvent().addHandler([](const ButtonHandler::ButtonEventType e, const ButtonHandler::Button b) {
        Serial.printf("Test button event handler: e = %d, b = %d\r\n", (int)e, (int)b);
    });

    Serial.printf("Test button event handler registered: %s\r\n", (registered ? "Yes" : "No"));

    registered = s_temperatureSensor.temperatureChangedEvent().addHandler([](const int16_t temp) {
        Serial.printf("Test temperature changed event handler: temp = %d\r\n", temp);
    });

    Serial.printf("Test temperature changed event handler registered: %s\r\n", (registered ? "Yes" : "No"));

    s_clock.epochUpdatedEvent().addHandler([](const uint32_t epoch) {
        Serial.printf("Test epoch updated event handler: epoch = %u\r\n", epoch);
    });

    Serial.println("Setting up main loop");

    runTasks = [] {
        s_blynkHandler.task();
        s_buttonHandler.task();
        s_temperatureSensor.task();
        s_relayController.task();
        s_clock.task();
        s_automator.task();
    };

    Serial.printf("runTasks = %p\r\n", runTasks);

    Serial.println("Setup finished");
}

void loop()
{
    runTasks();
}