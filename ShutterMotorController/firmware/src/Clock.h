#pragma once

#include "Config.h"
#include "Event.h"
#include "Logger.h"

#include <cstdint>
#include <ctime>
#include <NTPClient.h>
#include <WiFiUdp.h>

class Clock
{
public:
    Clock();

    void task();

    using EpochUpdatedEvent = Event<Config::Limits::MaxClockEpochUpdatedEventHandlers, const uint32_t>;
    const EpochUpdatedEvent& epochUpdatedEvent() const;

    std::time_t epochTime() const;
    const std::tm* const gmtime() const;

    bool isSynchronized() const;

private:
    const Logger m_log;
    WiFiUDP m_udpSocket;
    NTPClient m_ntpClient;
    EpochUpdatedEvent m_epochUpdatedEvent;
    uint32_t m_lastUpdateTime = 0;
    uint32_t m_updateInterval = 0;
    bool m_synchronized = false;
};