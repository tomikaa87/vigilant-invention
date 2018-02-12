#pragma once

#include "Config.h"
#include "Event.h"

#include <cstdint>

class BlynkParam;

class BlynkHandler
{
public:
    BlynkHandler();
    ~BlynkHandler();

    void task();

    void updateTemperature(const int16_t value);

    void onConnected();
    void onButtonPressed(const int pin);
    void onVirtualPinUpdated(const int pin, const BlynkParam& param);
    void updateVirtualPin(const int pin);

    enum class EventType
    {
        RelayButtonPressed
    };

    using BlynkEvent = Event<Config::Limits::MaxBlynkEventHandlers, const EventType, const int>;
    const BlynkEvent& blynkEvent() const;

private:
    int16_t m_lastRoomTemperature = 0;
    BlynkEvent m_event;
};