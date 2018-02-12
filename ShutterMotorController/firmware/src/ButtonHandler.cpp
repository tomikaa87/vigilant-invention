#include "ButtonHandler.h"

#include <Arduino.h>

ButtonHandler::ButtonHandler()
{
    static_assert(Config::ButtonReadInterval > 0, "ButtonReadInterval must be greater than 0");
    static_assert(Config::ButtonLongPressInterval > 0, "ButtonLongPressInterval must be greater than 0");

    ::pinMode(Config::Pins::Adc, INPUT);
}

const ButtonHandler::ButtonEvent& ButtonHandler::buttonEvent() const
{
    return m_buttonEvent;
}

void ButtonHandler::task()
{
    auto currentTime = ::millis();

    if (currentTime - m_lastReadTime < Config::ButtonReadInterval)
        return;

    m_lastReadTime = currentTime;

    auto analogValue = ::analogRead(Config::Pins::Adc);
    auto pressedButton = readButtonFromAdcValue(analogValue);

    if (m_lastPressedButton != Button::None)
    {
        if (pressedButton == Button::None)
        {
#ifdef DEBUG_BUTTON_HANDLER
            Serial.printf("ButtonHandler: ADC value: %d, Button: none\r\n",
                          analogValue);
#endif

            m_buttonEvent(ButtonEventType::Released, pressedButton);

            m_buttonLongPressed = false;
            m_lastPressedButton = Button::None;
        }
        else if (!m_buttonLongPressed && ::millis() - m_lastPressTime >= Config::ButtonLongPressInterval)
        {
            m_buttonLongPressed = true;

            m_buttonEvent(ButtonEventType::LongPressed, pressedButton);
        }
    }
    else
    {
        if (pressedButton != Button::None)
        {
            m_lastPressedButton = pressedButton;
            m_lastPressTime = ::millis();

#ifdef DEBUG_BUTTON_HANDLER
            Serial.printf("ButtonHandler: ADC value: %d, Button: %d\r\n",
                          analogValue,
                          (int)(pressedButton));
#endif

            m_buttonEvent(ButtonEventType::Pressed, pressedButton);
        }
    }
}

ButtonHandler::Button ButtonHandler::readButtonFromAdcValue(int value)
{
    if (value <= Config::ButtonAnalogThresholds::None)
        return Button::None;

    if (value <= Config::ButtonAnalogThresholds::Button1)
        return Button::Button1;

    if (value <= Config::ButtonAnalogThresholds::Button2)
        return Button::Button2;

    if (value <= Config::ButtonAnalogThresholds::Button3)
        return Button::Button3;

    if (value <= Config::ButtonAnalogThresholds::Button4)
        return Button::Button4;

    return Button::None;
}
