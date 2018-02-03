#pragma once

#include <Arduino.h>

namespace drivers
{

namespace gpio
{

template <uint8_t PinNumber>
struct InputPin
{
    static void setup()
    {
        pinMode(PinNumber, INPUT);
    }

    static bool isActive()
    {
        return digitalRead(PinNumber);
    }
};

template <uint8_t PinNumber>
struct InputPinWithPullUp
{
    static void setup()
    {
        pinMode(PinNumber, INPUT_PULLUP);
    }

    static bool isActive()
    {
        return digitalRead(PinNumber);
    }
};

template <typename InputPinClass>
struct InverseInput
{
    static void setup()
    {
        InputPinClass::setup();
    }

    static bool isActive()
    {
        return !InputPinClass::isActive();
    }
};

}

}