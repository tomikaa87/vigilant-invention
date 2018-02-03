#pragma once

#include "gpio.hpp"

#include <cstdint>

namespace drivers
{
    
class Keypad
{
public:
    Keypad();

    enum class Event : uint8_t
    {
        Nothing,
        Button1Pressed,
        Button1Released,
        Button2Pressed,
        Button2Released,
        Button3Pressed,
        Button3Released,
        Button4Pressed,
        Button4Released
    };

    Event task();

private:
    struct
    {
        uint8_t button1 : 1;
        uint8_t button2 : 1;
        uint8_t button3 : 1;
        uint8_t button4 : 1;
        uint8_t : 0;
    }  m_buttonStates = { 0 };

    using button1 = gpio::InverseInput<gpio::InputPinWithPullUp<12>>;
    using button2 = gpio::InverseInput<gpio::InputPinWithPullUp<14>>;
    using button3 = gpio::InputPin<15>;
    using button4 = gpio::InverseInput<gpio::InputPinWithPullUp<2>>;
};

}