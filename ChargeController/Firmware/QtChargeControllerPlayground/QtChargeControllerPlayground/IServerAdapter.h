#pragma once

#include "NetworkEndpoint.h"

#include <cstdint>
#include <functional>

class IServerAdapter
{
public:
    using ClientConnectedHandler = std::function<void(const uint16_t endpointId)>;
    using ClientDisconnectedHandler = std::function<void(const uint16_t endpointId)>;
    using ClientDataReceivedHandler = std::function<void(const uint16_t endpointId)>;

    virtual void onClientConnected(ClientConnectedHandler&& handler) = 0;
    virtual void onClientDisconnected(ClientDisconnectedHandler&& handler) = 0;
    virtual void onClientDataReceived(ClientDataReceivedHandler&& handler) = 0;
    virtual size_t availableDataBytes(const uint16_t endpointId) const = 0;
    virtual size_t sendDataToClient(const uint16_t endpointId, const char* const data, const size_t length) = 0;
    virtual size_t readClientData(const uint16_t endpointId, char* data, const size_t length) = 0;
};