#pragma once

#include "IServerAdapter.h"

#include <ESP8266WiFi.h>
#include <list>

class WiFiServerAdapter : public IServerAdapter
{
public:
    WiFiServerAdapter();

    static void connect();
    void start();
    void task();

    // Inherited via IServerAdapter
    virtual void onClientConnected(ClientConnectedHandler&& handler) override;
    virtual void onClientDisconnected(ClientDisconnectedHandler&& handler) override;
    virtual void onClientDataReceived(ClientDataReceivedHandler&& handler) override;
    virtual size_t availableDataBytes(const uint16_t endpointId) const override;
    virtual size_t sendDataToClient(const uint16_t endpointId, const char* const data, const size_t length) override;
    virtual size_t readClientData(const uint16_t endpointId, char* data, const size_t length) override;

private:
    WiFiServer m_server;

    uint16_t m_nextEndpointId = 0;

    struct Endpoint
    {
        uint16_t id;
        WiFiClient client;
    };

    std::list<Endpoint> m_endpoints;

    IServerAdapter::ClientConnectedHandler m_clientConnectedHandler;
    IServerAdapter::ClientDisconnectedHandler m_clientDisconnectedHandler;
    IServerAdapter::ClientDataReceivedHandler m_clientDataReceivedHandler;

    WiFiClient* findClientByEndpointId(const uint16_t endpointId) const;
};

