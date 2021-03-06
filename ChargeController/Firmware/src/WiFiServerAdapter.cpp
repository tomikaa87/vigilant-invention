#include "WiFiServerAdapter.h"
#include "PrivateConfig.h"

#include <Arduino.h>

WiFiServerAdapter::WiFiServerAdapter()
    : m_server{ 52461 }
{}

void WiFiServerAdapter::connect()
{
    Serial.println("Connecting to WiFi");

    if (WiFi.isConnected())
    {
        Serial.println("WiFi is already connected");
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.setPhyMode(WIFI_PHY_MODE_11N);

    WiFi.begin(PrivateConfig::WiFiSsid, PrivateConfig::WiFiPassword);

    WiFi.onStationModeConnected([](const WiFiEventStationModeConnected& e) {
        Serial.printf("WiFi connected. Channel: %d\r\n", e.channel);
    });

    WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& e) {
        Serial.printf("WiFi disconnected. Reason: %d\r\n", e.reason);
    });

    auto result = WiFi.waitForConnectResult();

    Serial.printf("WiFi connect result: %d\r\n", result);
}

void WiFiServerAdapter::start()
{
    Serial.println("Starting the server on port 52461");

    m_server.begin();
}

void WiFiServerAdapter::task()
{
    while (m_server.hasClient())
    {
        auto&& client = m_server.available();

        Serial.print("Client connected: ");
        Serial.print(client.remoteIP().toString());
        Serial.printf(":%d\r\n", client.remotePort());

        if (client.remoteIP() == 0u)
        {
            Serial.println("Dropping connection from invalid source IP");
            client.stop();

            return;
        }

        if (m_endpoints.size() == 2)
        {
            Serial.println("Maximum number of simultaneous connections reached, dropping connection");
            client.stop();

            return;
        }

        auto endpointId = m_nextEndpointId++;

        m_endpoints.emplace_back(endpointId, std::move(client));

        Serial.printf("Connected endpoints: %d\r\n", m_endpoints.size());

        if (m_clientConnectedHandler)
            m_clientConnectedHandler(endpointId);
    }

    for (auto it = std::begin(m_endpoints); it != std::end(m_endpoints);)
    {
        if (!it->client.connected())
        {
            // TODO shows 0.0.0.0:0 on the console
            Serial.print("Client disconnected: ");
            Serial.print(it->client.remoteIP().toString());
            Serial.printf(":%d\r\n", it->client.remotePort());

            if (m_clientDisconnectedHandler)
                m_clientDisconnectedHandler(it->id);

            it->client.stop();

            it = m_endpoints.erase(it);
            continue;
        }

        int available = it->client.available();

        if (available == 0 || !m_clientDataReceivedHandler)
            continue;

        Serial.printf("Received %d byte(s) from ", available);
        Serial.print(it->client.remoteIP().toString());
        Serial.printf(":%d\r\n", it->client.remotePort());

        m_clientDataReceivedHandler(it->id);

        Serial.println();
    }
}

void WiFiServerAdapter::onClientConnected(ClientConnectedHandler&& handler)
{
    m_clientConnectedHandler = std::move(handler);
}

void WiFiServerAdapter::onClientDisconnected(ClientDisconnectedHandler&& handler)
{
    m_clientDisconnectedHandler = std::move(handler);
}

void WiFiServerAdapter::onClientDataReceived(ClientDataReceivedHandler&& handler)
{
    m_clientDataReceivedHandler = std::move(handler);
}

std::size_t WiFiServerAdapter::availableDataBytes(const uint16_t endpointId) const
{
    auto&& client = findClientByEndpointId(endpointId);
    if (!client)
        return 0;

    return client->available();
}

std::size_t WiFiServerAdapter::sendDataToClient(const uint16_t endpointId, const char* const data, const std::size_t length)
{
    auto&& client = findClientByEndpointId(endpointId);
    if (!client)
        return 0;

    return client->write(data, length);
}

std::size_t WiFiServerAdapter::readClientData(const uint16_t endpointId, char* data, const std::size_t length)
{
    auto&& client = findClientByEndpointId(endpointId);
    if (!client)
        return 0;

    return client->read(reinterpret_cast<uint8_t*>(data), length);
}

WiFiClient* WiFiServerAdapter::findClientByEndpointId(const uint16_t endpointId) const
{
    auto&& it = std::find_if(
        std::begin(m_endpoints),
        std::end(m_endpoints),
        [endpointId](const Endpoint& ep) { return ep.id == endpointId; });

    if (it != std::end(m_endpoints))
        // FIXME const_cast is necessary because some methods in the client are not const-correct
        return const_cast<WiFiClient*>(&it->client);

    return nullptr;
}
