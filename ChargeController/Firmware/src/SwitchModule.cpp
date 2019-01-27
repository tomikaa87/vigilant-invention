#include "SwitchModule.h"
#include "hardware.h"

#include <Arduino.h>

SwitchModule::SwitchModule()
{
    setState(false);
}

void SwitchModule::setState(const bool on)
{
    Serial.printf("Switch, on = %d\r\n", on ? 1 : 0);

    digitalWrite(Pins::Relay, on ? HIGH : LOW);

    m_state = on;
}

bool SwitchModule::state() const
{
    return m_state;
}
