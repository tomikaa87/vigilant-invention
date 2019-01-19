#pragma once

#include <functional>

class Timer
{
public:
    void task();

    void restart(const int timeoutSeconds);

    using TimeoutCallback = std::function<void()>;

    void setTimeoutCallback(TimeoutCallback&& callback);

private:
    TimeoutCallback m_timeoutCallback;
    int m_timeoutSeconds = -1;
    int m_startTime = 0;
};