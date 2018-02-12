#include "RelayController.h"
#include "Config.h"

#include <Arduino.h>

RelayController::RelayController()
{
    static_assert(sizeof(Config::Pins::Relay) > 0, "No relay pins defined");
    static_assert(sizeof(Config::Pins::Relay) <= 255, "Maximum 255 relays supported");

    uint8_t i = 0;

    for (const auto pin : Config::Pins::Relay)
    {
        Serial.printf("RelayController: Setting up GPIO%d for relay %u\r\n", pin, i);

        pinMode(pin, OUTPUT);
        setState(i, false);

        ++i;
    }
}

void RelayController::switchOn(const uint8_t relay)
{
    Serial.printf("RelayController: switching on relay %u\r\n", relay);

    setState(relay, true);
}

void RelayController::switchOff(const uint8_t relay)
{
    Serial.printf("RelayController: switching off relay %u\r\n", relay);

    setState(relay, false);
}

void RelayController::setState(const uint8_t relay, const bool on)
{
    if (relay >= sizeof(Config::Pins::Relay))
    {
        Serial.printf("RelayController: invalid relay index: %u\r\n", relay);
        return;
    }

    Serial.printf("RelayController: setting state of relay %u to %s\r\n", relay, (on ? "On" : "Off"));

    digitalWrite(Config::Pins::Relay[relay], on ? 1 : 0);
}

void RelayController::toggle(const uint8_t relay)
{
    Serial.printf("RelayController: toggling relay %u\r\n", relay);

    setState(relay, digitalRead(Config::Pins::Relay[relay]) ? false : true);
}

void RelayController::pulse(const uint8_t relay)
{
    Serial.printf("RelayController: pulsing relay %u\r\n", relay);

    if (relay >= sizeof(Config::Pins::Relay))
    {
        Serial.printf("RelayController: invalid relay index: %u\r\n", relay);
        return;
    }

    if (m_pulseStates[relay])
    {
        Serial.printf("RelayController: relay %u is already being pulsed\r\n", relay);
        return;
    }

    m_pulseStartTimes[relay] = millis();
    m_pulseStates[relay] = true;

    switchOn(relay);
}

void RelayController::task()
{
    for (uint8_t i = 0; i < sizeof(Config::Pins::Relay); ++i)
    {
        if (m_pulseStates[i] && ::millis() - m_pulseStartTimes[i] >= Config::Relays::PulseWidth)
        {
            switchOff(i);
            m_pulseStates[i] = false;
        }
    }
}
