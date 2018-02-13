#include "Clock.h"

#include <Arduino.h>

Clock::Clock()
    : m_ntpClient(m_udpSocket,
                  Config::Clock::NtpServerAddress,
                  Config::Clock::LocalTimeOffsetMinutes * 60)
{
    Serial.printf("Clock: setting up NTP: server: %s, offset: %d min, update interval: %d min\r\n",
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
        m_updateInterval = Config::Clock::NtpUpdateIntervalMinutes * 60000;
        
        const auto epoch = m_ntpClient.getEpochTime();
        m_epochUpdatedEvent(epoch);

        Serial.printf("Clock: NTP updated, epoch = %u\r\n", epoch);
    }
    else
    {
        m_updateInterval = Config::Clock::NtpUpdateRetryIntervalSeconds * 1000;

        Serial.printf("Clock: NTP update failed, retrying after %d minute(s)\r\n", m_updateInterval / 1000);
    }
}

const Clock::EpochUpdatedEvent& Clock::epochUpdatedEvent() const
{
    return m_epochUpdatedEvent;
}
