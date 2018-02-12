#pragma once

#include <Arduino.h>
#include <cstdint>

namespace Config
{

    namespace Pins
    {
        static const int Adc = PIN_A0;
        static const int OneWire = 13;

        static const uint8_t Relay[] = { 4, 5, 14, 12 };
    }

    namespace ButtonAnalogThresholds
    {
        static const int None = 100;
        static const int Button1 = 250;
        static const int Button2 = 500;
        static const int Button3 = 750;
        static const int Button4 = 1024;
    }

    static const uint8_t DS18B20Resolution = 12;
    static const uint32_t TemperatureSensorReadInterval = 1000;

    static const uint32_t ButtonReadInterval = 100;
    static const uint32_t ButtonLongPressInterval = 1000;

    namespace Limits
    {
        static const size_t MaxButtonEventHandlers = 5;
        static const size_t MaxTemperatureEventHandlers = 5;
    }

    namespace Blynk
    {
        static const int RoomTemperaturePin = 80;
    }

    namespace Relays
    {
        static const uint32_t PulseWidth = 500;
    }

}

//#define DEBUG_BUTTON_HANDLER
//#define DEBUG_TEMPERATURE_SENSOR
#define DEBUG_BLYNK_HANDLER