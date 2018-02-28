#pragma once

template <size_t HandlerLimit, typename... EventArgs>
struct Event
{
    Event()
    {
        static_assert(HandlerLimit > 0, "HandlerLimit must be greater than 0");
    }

    Event(Event&&) = default;
    Event(const Event&) = delete;

    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&) = default;

    using HandlerFunc = void(*)(EventArgs... args);

    bool addHandler(HandlerFunc handler) const
    {
        for (auto it = std::begin(handlers); it != std::end(handlers); ++it)
        {
            if (!*it)
            {
                *it = handler;
                return true;
            }
        }

        return false;
    }

    bool removeHandler(HandlerFunc handler) const
    {
        for (auto it = std::begin(handlers); it != std::end(handlers); ++it)
        {
            if (*it == handler)
            {
                *it = nullptr;
                return true;
            }
        }

        return false;
    }

    void operator()(EventArgs... args) const
    {
        for (auto handler : handlers)
        {
            if (handler)
                handler(args...);
        }
    }

    mutable HandlerFunc handlers[HandlerLimit] = { 0 };
};