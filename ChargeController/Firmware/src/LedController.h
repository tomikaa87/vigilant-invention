#pragma once

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

    void setLed(Led led, State state, unsigned int timeout = 0);

private:
    struct LedInfo final
    {
        State state = State::Off;
        unsigned int onTime = 0;
        unsigned int timeout = 0;
        const int pin;

        LedInfo(const int pin)
            : pin(pin)
        {}
    };

    LedInfo m_ledInfo[LED_COUNT];

    LedInfo& getLedInfo(const Led led);
};