#pragma once

#include <cstdint>
#include <functional>

class Timer final
{
public:
    void task();

    void restart(const int timeoutSeconds);

    using TimeoutCallback = std::function<void()>;

    void setTimeoutCallback(TimeoutCallback&& callback);

private:
    TimeoutCallback m_timeoutCallback;
    uint32_t m_timeoutSeconds = 0;
    uint32_t m_startTime = 0;
};