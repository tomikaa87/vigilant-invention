#pragma once

#include "Command.h"
#include "DeviceIndex.h"

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
};

}
