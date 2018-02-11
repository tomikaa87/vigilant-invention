#pragma once

#include "Config.h"
#include "Event.h"

#include <cstdint>
#include <OneWire.h>

class TemperatureSensor
{
public:
    TemperatureSensor();

    void task();

    int16_t temperature() const;

    using TemperatureChangedEvent = Event<Config::Limits::MaxTemperatureEventHandlers, const int16_t>;
    const TemperatureChangedEvent& temperatureChangedEvent() const;

private:
    OneWire m_oneWire;
    int16_t m_lastValue = 8500;
    uint32_t m_lastReadTime = 0;
    TemperatureChangedEvent m_temperatureChangedEvent;

    int16_t read();
};