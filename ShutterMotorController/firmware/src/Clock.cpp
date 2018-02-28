#include "Clock.h"

#include <Arduino.h>

Clock::Clock()
    : m_log{ "Clock" }
    , m_ntpClient(m_udpSocket,
                  Config::Clock::NtpServerAddress,
                  Config::Clock::LocalTimeOffsetMinutes * 60)
{
    m_log.debug("Clock: setting up NTP: server: %s, offset: %d min, update interval: %d min",
                Config::Clock::NtpServerAddress,
                Config::Clock::LocalTimeOffsetMinutes,
                Config::Clock::NtpUpdateIntervalMinutes);

    m_ntpClient.begin();
}

void Clock::task()
{
    // Result of NTPClient::update() doesn't indicate if there was an update in
    // the current iteration so we handle periodic updates manually.

    if (::millis() - m_lastUpdateTime < m_updateInterval)
        return;

    m_lastUpdateTime = ::millis();

    if (m_ntpClient.forceUpdate())
    {
        m_synchronized = true;
        m_updateInterval = Config::Clock::NtpUpdateIntervalMinutes * 60000;
        
        const auto epoch = m_ntpClient.getEpochTime();
        m_epochUpdatedEvent(epoch);

        m_log.debug("Clock: NTP updated, epoch = %u", epoch);
    }
    else
    {
        m_updateInterval = Config::Clock::NtpUpdateRetryIntervalSeconds * 1000;

        m_log.warning("Clock: NTP update failed, retrying in %d minute(s)",
                      m_updateInterval / 1000);
    }
}

const Clock::EpochUpdatedEvent& Clock::epochUpdatedEvent() const
{
    return m_epochUpdatedEvent;
}

std::time_t Clock::epochTime() const
{
    return m_ntpClient.getEpochTime();
}

const std::tm* const Clock::gmtime() const
{
    const auto epoch = epochTime();
    return std::gmtime(&epoch);
}

bool Clock::isSynchronized() const
{
    return m_synchronized;
}
