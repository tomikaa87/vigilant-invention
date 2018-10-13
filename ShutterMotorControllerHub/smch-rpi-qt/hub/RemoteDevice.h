#pragma once

#include <string>

namespace hub
{

struct RemoteDevice
{
    int packetsSent = 0;
    int packetsLost = 0;
    std::string firmwareVersion;
    std::string name;
};

}
