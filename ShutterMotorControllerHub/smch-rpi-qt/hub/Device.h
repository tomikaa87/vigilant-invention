#pragma once

#include <string>

namespace hub
{

struct Device
{
    std::string address;
    std::string firmwareVersion;

    Device(std::string&& address)
        : address(std::move(address))
    {}

    Device(const std::string& address)
        : address{ address }
    {}
};

}
