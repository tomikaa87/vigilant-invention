#include "LedController.h"
#include "hardware.h"

#include <Arduino.h>

LedController::LedController()
    : m_ledInfo{
        LedInfo{ Pins::RedLed },
        LedInfo{ Pins::GreenLed }
    }
{
    pinMode(Pins::RedLed, OUTPUT);
    pinMode(Pins::GreenLed, OUTPUT);

    setLed(Led::Red, State::Off);
    setLed(Led::Green, State::Off);
}

void LedController::task()
{
    for (auto&& ledInfo : m_ledInfo)
    {
        if (ledInfo.state == State::On
            && ledInfo.timeout > 0
            && ledInfo.onTime + ledInfo.timeout < millis())
        {
            ledInfo.state = State::Off;
            digitalWrite(ledInfo.pin, 0);
        }
    }
}

void LedController::setLed(const Led led, const State state, const unsigned timeout)
{
    auto&& ledInfo = getLedInfo(led);

    ledInfo.state = state;

    if (state == State::On)
    {
        ledInfo.onTime = millis();
        ledInfo.timeout = timeout;
    }

    digitalWrite(ledInfo.pin, ledInfo.state == State::On ? 1 : 0);
}

LedController::LedInfo& LedController::getLedInfo(const Led led)
{
    return m_ledInfo[static_cast<int>(led)];
}
