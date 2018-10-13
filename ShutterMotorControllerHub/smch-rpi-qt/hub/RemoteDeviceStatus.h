#pragma once

#include <string>
#include <vector>

#ifdef RASPBERRY_PI
#include "radio_protocol.h"
#endif

namespace hub
{

struct RemoteDeviceStatus
{
    std::string firmwareVersion;

#ifdef RASPBERRY_PI
    std::vector<protocol_cmd_t> lastCommands;
#else
    std::vector<uint8_t> lastCommands;
#endif
};

}
