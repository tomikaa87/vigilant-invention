#pragma once

#include <string>

namespace hub
{

struct Device
{
    std::string address;

    Device(std::string&& address)
        : address(std::move(address))
    {}

    Device(const Device& o) = delete;
    Device(Device&& o) = default;

    Device& operator=(const Device& o) = delete;
    Device& operator=(Device&& o) = default;
};

}
