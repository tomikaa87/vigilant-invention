#pragma once

#include "IdeHelper.h"

#include <cstdint>
#include <functional>

class IServerAdapter
{
public:
    IServerAdapter() = default;
    IServerAdapter(const IServerAdapter&) = default;
    IServerAdapter(IServerAdapter&&) = default;

    IServerAdapter& operator=(const IServerAdapter&) = default;
    IServerAdapter& operator=(IServerAdapter&&) = default;

    virtual ~IServerAdapter() = default;

    using ClientConnectedHandler = std::function<void(const uint16_t endpointId)>;
    using ClientDisconnectedHandler = std::function<void(const uint16_t endpointId)>;
    using ClientDataReceivedHandler = std::function<void(const uint16_t endpointId)>;

    virtual void onClientConnected(ClientConnectedHandler&& handler) = 0;
    virtual void onClientDisconnected(ClientDisconnectedHandler&& handler) = 0;
    virtual void onClientDataReceived(ClientDataReceivedHandler&& handler) = 0;
    virtual std::size_t availableDataBytes(const uint16_t endpointId) const = 0;
    virtual std::size_t sendDataToClient(const uint16_t endpointId, const char* const data, const std::size_t length) = 0;
    virtual std::size_t readClientData(const uint16_t endpointId, char* data, const std::size_t length) = 0;
};