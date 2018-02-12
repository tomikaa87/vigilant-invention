#pragma once

#include <cstdint>

class RelayController
{
public:
    RelayController();

    void switchOn(const uint8_t relay);
    void switchOff(const uint8_t relay);
    void setState(const uint8_t relay, const bool on);
    void toggle(const uint8_t relay);
    void pulse(const uint8_t relay);

    void task();

private:
    bool m_pulseStates[4] = { false };
    uint32_t m_pulseStartTimes[4] = { 0 };
};