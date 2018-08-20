#ifndef IHUB_H
#define IHUB_H

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "radio_protocol.h"

class IHub
{
public:
    struct RemoteDeviceStatus
    {
        std::string firmwareVersion;
        std::vector<protocol_cmd_t> lastCommands;
    };

    struct RemoteDevice
    {
        int packetsSent = 0;
        int packetsLost = 0;
        std::string firmwareVersion;
        std::string name;
    };

    virtual ~IHub() = default;



    virtual void readStatus(std::function<void(RemoteDeviceStatus&& status)>&& callback = {}) = 0;
    virtual void shutter1Up() = 0;
    virtual void shutter1Down() = 0;
    virtual void shutter2Up() = 0;
    virtual void shutter2Down() = 0;

    virtual void scanUnits(std::function<void()>&& callback = {}) = 0;

    virtual const std::unordered_map<uint8_t, RemoteDevice>& devices() const = 0;

    virtual void selectDevice(uint8_t index) = 0;
};

#endif // IHUB_H
