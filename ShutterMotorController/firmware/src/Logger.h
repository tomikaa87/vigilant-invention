#pragma once

#include "Config.h"
#include "Event.h"

class Clock;

class Logger
{
public:
    enum class Severity
    {
        Debug,
        Info,
        Warning,
        Error
    };

    static void setClock(const Clock* clock);

    using LogEvent = Event<Config::Limits::MaxLogEventHandlers, const Severity, const char*>;
    static const LogEvent& logEvent();

    Logger() = default;
    Logger(const char* category);

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;

    template <typename FirstArg, typename... Args>
    inline void log(const Severity severity, const char* fmt, FirstArg&& first, Args&&... arg) const
    {
        char buf[Config::Logger::LogLineBufferSize] = { 0 };
        auto length = addPrefixes(buf, sizeof(buf), severity);

        length += snprintf(buf + length, sizeof(buf) - length, fmt, std::forward<FirstArg>(first), std::forward<Args>(arg)...);
        snprintf(buf + length, sizeof(buf) - length, "\r\n");

        sm_logEvent(severity, buf);
    }

    inline void log(const Severity severity, const char* msg) const
    {
        log(severity, "%s", msg);
    }

    template <typename FirstArg, typename... Args>
    inline void debug(const char* fmt, FirstArg&& first, Args&&... arg) const
    {
        log(Severity::Debug, fmt, std::forward<FirstArg>(first), std::forward<Args>(arg)...);
    }

    inline void debug(const char* msg) const
    {
        log(Severity::Debug, msg);
    }

    template <typename FirstArg, typename... Args>
    inline void info(const char* fmt, FirstArg&& first, Args&&... arg) const
    {
        log(Severity::Info, fmt, std::forward<FirstArg>(first), std::forward<Args>(arg)...);
    }

    inline void info(const char* msg) const
    {
        log(Severity::Info, msg);
    }

    template <typename FirstArg, typename... Args>
    inline void warning(const char* fmt, FirstArg&& first, Args&&... arg) const
    {
        log(Severity::Warning, fmt, std::forward<FirstArg>(first), std::forward<Args>(arg)...);
    }

    inline void warning(const char* msg) const
    {
        log(Severity::Warning, msg);
    }

    template <typename FirstArg, typename... Args>
    inline void error(const char* fmt, FirstArg&& first, Args&&... arg) const
    {
        log(Severity::Error, fmt, std::forward<FirstArg>(first), std::forward<Args>(arg)...);
    }

    inline void error(const char* msg) const
    {
        log(Severity::Error, msg);
    }

private:
    static const Clock* sm_clock;
    static LogEvent sm_logEvent;
    const char* m_category = nullptr;

    size_t addPrefixes(char* buf, const size_t maxLength, const Severity severity) const;
};