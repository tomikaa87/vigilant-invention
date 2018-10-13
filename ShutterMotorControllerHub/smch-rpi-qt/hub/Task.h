#pragma once

#include "radio/Command.h"
#include "SubDevice.h"

#include <string>

namespace hub
{

struct Task
{
    radio::Command command;
    std::string address;

    static const int MaxRetryCount = 10;
    int retryCount = 0;

    Task(radio::Command command, std::string address)
        : command(command)
        , address(std::move(address))
        , m_valid(true)
    {}

    Task() = default;

    Task(const Task& o) = delete;
    Task(Task&& o) = default;

    Task& operator=(const Task& o) = delete;
    Task& operator=(Task&& o) = default;

    bool valid() const
    {
        return m_valid;
    }

private:
    bool m_valid = false;
};

}
