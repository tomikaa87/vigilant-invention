#include "WirelessController.h"

#include <Arduino.h>
#include <algorithm>

#include <json.hpp>

WirelessController::WirelessController()
    : m_server(52461)
{
}

void WirelessController::task()
{
    if (m_server.hasClient())
    {
        auto&& client = m_server.available();
        m_clients.push_back(std::move(client));

        Serial.print("Client connected: ");
        Serial.print(client.remoteIP().toString());
        Serial.printf(":%d\r\n", client.remotePort());
    }

    for (auto it = std::begin(m_clients); it != std::end(m_clients);)
    {
        if (!it->connected())
        {

            // TODO shows 0.0.0.0:0 on the console
            Serial.print("Client disconnected: ");
            Serial.print(it->remoteIP().toString());
            Serial.printf(":%d\r\n", it->remotePort());

            it = m_clients.erase(it);
            continue;
        }

        int available = it->available();

        if (available == 0)
            continue;

        Serial.printf("Received %d byte(s) from ", available);
        Serial.print(it->remoteIP().toString());
        Serial.printf(":%d\r\n", it->remotePort());

        while (it->available())
        {
            char buf[1024] = { 0 };
            /*int read = */it->read(reinterpret_cast<uint8_t*>(buf), sizeof(buf));
            Serial.print(buf);
        }

        Serial.println();
    }
}

void WirelessController::connectToWiFi()
{
    Serial.println("Connecting to WiFi");

    if (WiFi.isConnected())
    {
        Serial.println("WiFi is already connected");
        return;
    }

    WiFi.setAutoConnect(true);
    WiFi.setPhyMode(WIFI_PHY_MODE_11N);

    WiFi.begin("Szilvasbukta-2G", "@N4gy0nSz3r3tl3kCs0pp@");

    WiFi.onStationModeConnected([](const WiFiEventStationModeConnected& e) {
        Serial.printf("WiFi connected. Channel: %d\r\n", e.channel);
    });

    WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& e) {
        Serial.printf("WiFi disconnected. Reason: %d\r\n", e.reason);
    });

    auto result = WiFi.waitForConnectResult();

    Serial.printf("WiFi connect result: %d\r\n", result);

    nlohmann::json j = {
        { "test", 13 }
    };
    Serial.println(j.dump().c_str());
}

void WirelessController::startServer()
{
    Serial.println("Starting the server on port 52461");

    m_server.begin();
}
