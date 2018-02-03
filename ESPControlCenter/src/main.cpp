#include <Arduino.h>

#include "ui/ui.hpp"
#include "drivers/keypad.hpp"
#include "gree/ac.hpp"
#include "private_config.hpp"

drivers::Keypad* g_keypadDriver;
ui::UiHandler* g_uiHandler;
gree::AC* g_acKitchen;
gree::AC* g_acBedroom;

void setup() 
{
    Serial.begin(74880);

    static drivers::Keypad s_keypadDriver;
    g_keypadDriver = &s_keypadDriver;

    static ui::UiHandler s_uiHandler;
    g_uiHandler = &s_uiHandler;

    // static gree::AC s_acKitchen{ config::KitchenAcKey, config::KitchenAcAddress };
    // g_acKitchen = &s_acKitchen;

    // static gree::AC s_acBedroom{ config::BedroomAcKey, config::BedroomAcAddress };
    // g_acBedroom = &s_acBedroom;
}

void loop() 
{
    auto keypadEvent = g_keypadDriver->task();
    g_uiHandler->task(keypadEvent);

    static bool done = false;

    if (millis() > 1000 && !done)
    {
        done = true;

        static gree::AC s_acKitchen{ config::KitchenAcKey, config::KitchenAcAddress };
        g_acKitchen = &s_acKitchen;

        static gree::AC s_acBedroom{ config::BedroomAcKey, config::BedroomAcAddress };
        g_acBedroom = &s_acBedroom;
    }
}