#include "Logger.h"
#include "Clock.h"

#include <cstdio>

const Clock* Logger::sm_clock = nullptr;
Logger::LogEvent Logger::sm_logEvent;

void Logger::setClock(const Clock* clock)
{
    sm_clock = clock;
}

const Logger::LogEvent& Logger::logEvent()
{
    return sm_logEvent;
}

Logger::Logger(const char* category)
    : m_category(category)
{
}

size_t Logger::addPrefixes(char* buf, const size_t maxLength, const Severity severity) const
{
    size_t length = 0;

    if (sm_clock)
    {
        auto time = sm_clock->gmtime();

        length = snprintf(buf, maxLength, "%04d-%02d-%02d %02d:%02d:%02d: ",
                          time->tm_year + 1900,
                          time->tm_mon + 1,
                          time->tm_mday,
                          time->tm_hour,
                          time->tm_min,
                          time->tm_sec);
    }

    if (m_category)
    {
        length += snprintf(buf + length, maxLength - length, "[%s] ",
                           m_category);
    }

    const char* severityString = "";

    switch (severity)
    {
        case Severity::Debug:
            severityString = "{D} ";
            break;

        case Severity::Warning:
            severityString = "{W} ";
            break;

        case Severity::Error:
            severityString = "{E} ";
            break;

        default:
            break;
    }

    length += snprintf(buf + length, sizeof(buf) - length, "%s",
                       severityString);

    return length;
}
