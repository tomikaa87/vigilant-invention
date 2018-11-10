#pragma once

#include "Command.h"
#include "Device.h"
#include "DeviceIndex.h"

#include <future>
#include <tuple>
#include <vector>

namespace hub
{

class IRemoteControl
{
public:
    virtual ~IRemoteControl() = default;

    virtual void execute(hub::Command command, DeviceIndex device) = 0;
    virtual void execute(hub::Command command, std::vector<DeviceIndex>&& devices) = 0;
    virtual void executeOnAll(hub::Command command) = 0;

    virtual std::future<std::tuple<bool, Device>> queryStatus(DeviceIndex device) = 0;
};

}
