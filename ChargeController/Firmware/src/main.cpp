#include <Arduino.h>

#include "hardware.h"

#include "ChargeControllerServer.h"
#include "WiFiServerAdapter.h"

static WiFiServerAdapter wifiServerAdapter;
static ChargeControllerServer server{ wifiServerAdapter };

void setup()
{
    Serial.begin(74880);

    pinMode(Pins::Relay, OUTPUT);
    pinMode(Pins::RedLed, OUTPUT);
    pinMode(Pins::GreenLed, OUTPUT);

    digitalWrite(Pins::Relay, LOW);

    digitalWrite(Pins::RedLed, HIGH);
    digitalWrite(Pins::GreenLed, LOW);

    wifiServerAdapter.connect();
    wifiServerAdapter.start();

    digitalWrite(Pins::RedLed, LOW);
    digitalWrite(Pins::GreenLed, HIGH);
}

void loop()
{
    wifiServerAdapter.task();
}