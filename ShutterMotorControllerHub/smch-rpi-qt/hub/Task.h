#ifndef TASK_H
#define TASK_H

#include "radio/IRadio.h"
#include <string>

namespace hub
{

struct Task
{
    radio::IRadio::Command command;
    std::string address;

    static const int MaxRetryCount = 10;
    int retryCount = 0;

    Task(radio::IRadio::Command command, std::string address)
        : command(command)
        , address(std::move(address))
    {}

    Task() = default;

    Task(const Task& o) = delete;
    Task(Task&& o) = default;

    Task& operator=(const Task& o) = delete;
    Task& operator=(Task&& o) = default;

    bool valid() const
    {
        return !address.empty();
    }
};

}

#endif // TASK_H
