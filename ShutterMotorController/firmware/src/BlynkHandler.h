#pragma once

#include "Config.h"
#include "Event.h"
#include "Logger.h"

#include <cstdint>

class BlynkParam;
class PersistentStorage;

class BlynkHandler
{
public:
    BlynkHandler(PersistentStorage& persistentStorage);
    ~BlynkHandler();

    void task();

    void updateTemperature(const int16_t value);

    void onConnected();
    void onDisconnected();

    void onButtonPressed(const int pin);
    void onVirtualPinUpdated(const int pin, const BlynkParam& param);
    void updateVirtualPin(const int pin);

    void addVirtualTerminalLogLine(const char* line) const;

    enum class EventType
    {
        RelayButtonPressed
    };

    using BlynkEvent = Event<Config::Limits::MaxBlynkEventHandlers, const EventType, const int>;
    const BlynkEvent& blynkEvent() const;

private:
    const Logger m_log;
    PersistentStorage& m_peristentStorage;
    bool m_connected = false;

    int16_t m_lastRoomTemperature = 0;
    BlynkEvent m_event;

    void handleOpenTimeInputChange(const BlynkParam& param);
    void updateOpenTimeInput();

    void handleShutterTimerActiveChange(const BlynkParam& param);
};