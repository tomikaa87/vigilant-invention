#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace hub
{

class IHub
{
public:
    virtual ~IHub() = default;

    virtual void scanDevices() = 0;
};

}
