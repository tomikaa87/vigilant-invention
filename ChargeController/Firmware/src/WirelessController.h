#pragma once

#include "IdeHelper.h"

#include <ESP8266WiFi.h>
#include <list>

class WirelessController final
{
public:
    WirelessController();

    void task();

    void connectToWiFi();
    void startServer();

private:
    WiFiServer m_server;
    std::list<WiFiClient> m_clients;
};