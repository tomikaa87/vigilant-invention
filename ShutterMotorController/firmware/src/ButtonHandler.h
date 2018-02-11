#pragma once

#include "Config.h"
#include "Event.h"

#include <cstdint>

class ButtonHandler
{
public:
    ButtonHandler();

    enum class ButtonEventType
    {
        Pressed,
        LongPressed,
        Released
    };

    enum class Button
    {
        None,

        Button1,
        Button2,
        Button3,
        Button4
    };

    using ButtonEvent = Event<Config::Limits::MaxButtonEventHandlers, const ButtonEventType, const Button>;
    const ButtonEvent& buttonEvent() const;

    void task();

private:
    uint32_t m_lastReadTime = 0;
    uint32_t m_lastPressTime = 0;
    bool m_buttonLongPressed = false;
    Button m_lastPressedButton = Button::None;
    ButtonEvent m_buttonEvent;

    Button readButtonFromAdcValue(int value);
};