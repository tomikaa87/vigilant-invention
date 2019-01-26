#include "SwitchModule.h"
#include "Arduino.h"
#include "hardware.h"

SwitchModule::SwitchModule()
{
    setState(false);
}

void SwitchModule::setState(bool on)
{
    Serial.printf("Switch, on = %d\r\n", on ? 1 : 0);

    digitalWrite(Pins::Relay, on ? HIGH : LOW);

    m_state = on;
}

bool SwitchModule::state() const
{
    return m_state;
}
