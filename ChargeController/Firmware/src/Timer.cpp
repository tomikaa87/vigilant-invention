#include <Arduino.h>

#include "Timer.h"

void Timer::task()
{
    if (m_timeoutSeconds > 0 && millis() > (m_startTime + m_timeoutSeconds * 1000))
    {
        m_timeoutSeconds = 0;

        if (m_timeoutCallback)
            m_timeoutCallback();
    }
}

void Timer::restart(const int timeoutSeconds)
{
    m_timeoutSeconds = timeoutSeconds;
    m_startTime = millis();
}

void Timer::setTimeoutCallback(TimeoutCallback&& callback)
{
    m_timeoutCallback = std::move(callback);
}
