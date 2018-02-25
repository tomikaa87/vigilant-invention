#pragma once

#include <cstdint>

class BlynkHandler;
class Clock;
class PersistentStorage;
class RelayController;

class Automator
{
public:
    Automator(const BlynkHandler& blynk,
              const Clock& clock,
              class PersistentStorage& persistentStorage,
              RelayController& relay);

    void task();

private:
    const BlynkHandler& m_blynk;
    const Clock& m_clock;
    const PersistentStorage& m_persistentStorage;
    RelayController& m_relay;

    uint32_t m_lastCheckTime = 0;

    enum class LastShutterEvent
    {
        None,
        Open,
        Close
    };

    LastShutterEvent m_lastShutterEvent = LastShutterEvent::None;

    void checkShutterOpenSchedule();

    void openShutters();
    void closeShutters();
};