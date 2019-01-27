#pragma once

#include "IdeHelper.h"

#include <cstdint>

class LedController final
{
    static constexpr int LED_COUNT = 2;

public:
    enum class Led: int
    {
        Red,
        Green
    };

    enum class State
    {
        On,
        Off
    };

    LedController();

    void task();

    void setLed(Led led, State state, uint32_t timeout = 0);

private:
    struct LedInfo final
    {
        State state = State::Off;
        uint32_t onTime = 0;
        uint32_t timeout = 0;
        const int pin;

        explicit LedInfo(const int pin)
            : pin(pin)
        {}
    };

    LedInfo m_ledInfo[LED_COUNT];

    LedInfo& getLedInfo(const Led led);
};