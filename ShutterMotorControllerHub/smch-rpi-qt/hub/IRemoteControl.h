#pragma once

#include "Command.h"
#include "DeviceIndex.h"

#include <future>
#include <vector>

namespace hub
{

class IRemoteControl
{
public:
    virtual ~IRemoteControl() = default;

    virtual std::shared_future<void> execute(hub::Command command, DeviceIndex device) = 0;
    virtual std::shared_future<void> execute(hub::Command command, std::vector<DeviceIndex>&& devices) = 0;
    virtual std::shared_future<void> executeOnAll(hub::Command command) = 0;
};

}
