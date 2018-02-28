#pragma once

#include "Logger.h"

#include <cstdint>

class PersistentStorage
{
public:
    PersistentStorage();

    void loadConfiguration();
    void saveConfiguration();
    void loadDefaults();

    struct __attribute__((__packed__)) PersistentConfig
    {
        bool ShutterTimerActive = false;

        uint8_t ShutterOpenHour = 0;
        uint8_t ShutterOpenMinute = 0;

        uint8_t ShutterCloseHour = 0;
        uint8_t ShutterCloseMinute = 0;
    };

    PersistentConfig config;

private:
    const Logger m_log;
    uint16_t m_lastChecksum = 0;

    uint16_t calculateConfigChecksum() const;
};