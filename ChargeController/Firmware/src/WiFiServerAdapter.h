#pragma once

#include "IdeHelper.h"

#include "IServerAdapter.h"

#include <ESP8266WiFi.h>
#include <list>

class WiFiServerAdapter final : public IServerAdapter
{
public:
    WiFiServerAdapter();

    static void connect();
    void start();
    void task();

    // Inherited via IServerAdapter
    void onClientConnected(ClientConnectedHandler&& handler) override;
    void onClientDisconnected(ClientDisconnectedHandler&& handler) override;
    void onClientDataReceived(ClientDataReceivedHandler&& handler) override;
    std::size_t availableDataBytes(const uint16_t endpointId) const override;
    std::size_t sendDataToClient(const uint16_t endpointId, const char* const data, const std::size_t length) override;
    std::size_t readClientData(const uint16_t endpointId, char* data, const std::size_t length) override;

private:
    WiFiServer m_server;

    uint16_t m_nextEndpointId = 0;

    struct Endpoint final
    {
        uint16_t id;
        WiFiClient client;

        Endpoint(const uint16_t id, WiFiClient&& wifiClient)
            : id{ id }
            , client{ std::move(wifiClient) }
        {}
    };

    std::list<Endpoint> m_endpoints;

    ClientConnectedHandler m_clientConnectedHandler;
    ClientDisconnectedHandler m_clientDisconnectedHandler;
    ClientDataReceivedHandler m_clientDataReceivedHandler;

    WiFiClient* findClientByEndpointId(const uint16_t endpointId) const;
};
