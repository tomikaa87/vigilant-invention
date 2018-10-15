#pragma once

#include <future>

namespace hub
{

class IHub
{
public:
    virtual ~IHub() = default;

    virtual std::future<void> scanDevices() = 0;
};

}
