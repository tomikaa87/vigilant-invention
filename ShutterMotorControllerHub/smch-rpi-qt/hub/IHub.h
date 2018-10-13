#pragma once

#include <cstdint>
#include <future>
#include <string>
#include <vector>

namespace hub
{

class IHub
{
public:
    virtual ~IHub() = default;

    virtual std::shared_future<void> scanDevices() = 0;
};

}
