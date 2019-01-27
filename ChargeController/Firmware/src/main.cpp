#include "hardware.h"
#include "ChargeControllerServer.h"
#include "WiFiServerAdapter.h"
#include "LedController.h"

#include <Arduino.h>

static LedController ledController;
static WiFiServerAdapter wifiServerAdapter;
static ChargeControllerServer server{ wifiServerAdapter, ledController };

void setup()
{
    Serial.begin(74880);

    pinMode(Pins::Relay, OUTPUT);
    digitalWrite(Pins::Relay, LOW);

    ledController.setLed(LedController::Led::Red, LedController::State::On);

    WiFiServerAdapter::connect();
    wifiServerAdapter.start();

    ledController.setLed(LedController::Led::Red, LedController::State::Off);
    ledController.setLed(LedController::Led::Green, LedController::State::On, 5000);
}

void loop()
{
    wifiServerAdapter.task();
    server.task();
    ledController.task();
}