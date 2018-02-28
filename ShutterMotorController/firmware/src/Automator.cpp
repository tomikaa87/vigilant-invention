#include "Automator.h"
#include "BlynkHandler.h"
#include "Clock.h"
#include "Config.h"
#include "PersistentStorage.h"
#include "RelayController.h"

#include <Arduino.h>
#include <ctime>
#include <EEPROM.h>

Automator::Automator(const Clock& clock,
                     class PersistentStorage& persistentStorage,
                     RelayController& relay)
    : m_log{ "Automator" }
    , m_clock(clock)
    , m_persistentStorage(persistentStorage)
    , m_relay(relay)
{
}

void Automator::task()
{
    if (::millis() - m_lastCheckTime < Config::Automator::CheckTimerInterval)
        return;

    m_lastCheckTime = ::millis();

    checkShutterOpenSchedule();
}

void Automator::checkShutterOpenSchedule()
{
    const auto& cfg = m_persistentStorage.config;

    if (!cfg.ShutterTimerActive)
        return;

    if (cfg.ShutterCloseHour == cfg.ShutterOpenHour && cfg.ShutterCloseMinute == cfg.ShutterOpenMinute)
        return;

    if (!m_clock.isSynchronized())
        return;

    const auto epochTime = m_clock.epochTime();
    const auto tm = std::gmtime(&epochTime);

    if (m_lastShutterEvent != LastShutterEvent::Open &&
        tm->tm_hour == cfg.ShutterCloseHour && tm->tm_min == cfg.ShutterCloseMinute)
    {
        m_log.debug("Shutter closing time point reached: %02d:%02d",
                    tm->tm_hour, tm->tm_min);

        m_lastShutterEvent = LastShutterEvent::Open;
        closeShutters();
    }

    if (m_lastShutterEvent != LastShutterEvent::Close &&
        tm->tm_hour == cfg.ShutterOpenHour && tm->tm_min == cfg.ShutterOpenMinute)
    {
        m_log.debug("Shutter opening time point reached: %02d:%02d",
                    tm->tm_hour, tm->tm_min);

        m_lastShutterEvent = LastShutterEvent::Close;
        openShutters();
    }
}

void Automator::openShutters()
{
    m_log.info("Opening shutters");

    m_relay.pulse(0);
    m_relay.pulse(2);
}

void Automator::closeShutters()
{
    m_log.info("Closing shutters");

    m_relay.pulse(1);
    m_relay.pulse(3);
}
