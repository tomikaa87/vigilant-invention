#pragma once

#include "Config.h"
#include "Logger.h"

#include <cstdint>

class RelayController
{
public:
    RelayController();

    void switchOn(const uint8_t relay) const;
    void switchOff(const uint8_t relay) const;
    void setState(const uint8_t relay, const bool on) const;
    void toggle(const uint8_t relay) const;
    void pulse(const uint8_t relay);

    bool isRelayTurnedOn(const uint8_t relay) const;
    bool isAnyRelayInTheSameGroupTurnedOn(const uint8_t relay) const;
    bool isValidRelayIndex(const uint8_t relay) const;

    void task();

private:
    const Logger m_log;

    bool m_pulseStates[sizeof(Config::Pins::Relay)] = { false };
    uint32_t m_pulseStartTimes[sizeof(Config::Pins::Relay)] = { 0 };
};