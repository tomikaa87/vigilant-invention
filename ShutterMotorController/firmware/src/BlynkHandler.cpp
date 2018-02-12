#include "BlynkHandler.h"
#include "PrivateConfig.h"

#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial

static BlynkHandler* g_instance = nullptr;

#define HANDLE_BLYNK_WRITE(__PIN) BLYNK_WRITE(__PIN) { \
    if (g_instance) \
        g_instance->onVirtualPinUpdated(__PIN, param); \
}

#define HANDLE_BLYNK_READ(__PIN) BLYNK_READ(__PIN) { \
    if (g_instance) \
        g_instance->updateVirtualPin(__PIN); \
}

#define HANDLE_BLYNK_BUTTON_PRESS(__PIN) BLYNK_WRITE(__PIN) { \
    if (g_instance && param.asInt() == 1) \
        g_instance->onButtonPressed(__PIN); \
}

BLYNK_CONNECTED()
{
    if (g_instance)
        g_instance->onConnected();
}

HANDLE_BLYNK_BUTTON_PRESS(CONFIG_BLYNK_PIN_RELAY_1_PULSE_BUTTON)
HANDLE_BLYNK_BUTTON_PRESS(CONFIG_BLYNK_PIN_RELAY_2_PULSE_BUTTON)
HANDLE_BLYNK_BUTTON_PRESS(CONFIG_BLYNK_PIN_RELAY_3_PULSE_BUTTON)
HANDLE_BLYNK_BUTTON_PRESS(CONFIG_BLYNK_PIN_RELAY_4_PULSE_BUTTON)

BlynkHandler::BlynkHandler()
{
    g_instance = this;

    Serial.println("BlynkHandler: starting Blynk");

    Blynk.begin(PrivateConfig::BlynkApiToken, PrivateConfig::WiFiSSID, PrivateConfig::WiFiPassword);

    Serial.println("BlynkHandler: Blynk started");
    Serial.println("BlynkHandler: configuring WiFi");

    WiFi.setPhyMode(WIFI_PHY_MODE_11N);

    Serial.println("BlynkHandler: WiFi configured");
}

BlynkHandler::~BlynkHandler()
{
    g_instance = nullptr;
}

void BlynkHandler::task()
{
    Blynk.run();
}

void BlynkHandler::updateTemperature(const int16_t value)
{
#ifdef DEBUG_BLYNK_HANDLER
    Serial.printf("BlynkHandler: updating temperature: %d\r\n", value);
#endif

    m_lastRoomTemperature = value;
    updateVirtualPin(CONFIG_BLYNK_PIN_ROOM_TEMPERATURE);
}

void BlynkHandler::onConnected()
{
    Serial.println("BlynkHandler: connected to Blynk");
}

void BlynkHandler::onButtonPressed(const int pin)
{
#ifdef DEBUG_BLYNK_HANDLER
    Serial.printf("BlynkHandler: button pressed: %d\r\n", pin);
#endif

    switch (pin)
    {
        case CONFIG_BLYNK_PIN_RELAY_1_PULSE_BUTTON:
            m_event(EventType::RelayButtonPressed, 0);
            break;

        case CONFIG_BLYNK_PIN_RELAY_2_PULSE_BUTTON:
            m_event(EventType::RelayButtonPressed, 1);
            break;

        case CONFIG_BLYNK_PIN_RELAY_3_PULSE_BUTTON:
            m_event(EventType::RelayButtonPressed, 2);
            break;

        case CONFIG_BLYNK_PIN_RELAY_4_PULSE_BUTTON:
            m_event(EventType::RelayButtonPressed, 3);
            break;

        default:
            break;
    }
}

void BlynkHandler::onVirtualPinUpdated(const int pin, const BlynkParam& param)
{
#ifdef DEBUG_BLYNK_HANDLER
    Serial.printf("BlynkHandler: virtual pin updated: %d to %s\r\n", pin, param.asString());
#endif
}

void BlynkHandler::updateVirtualPin(const int pin)
{
#ifdef DEBUG_BLYNK_HANDLER
    Serial.printf("BlynkHandler: virtual pin update requested: %d\r\n", pin);
#endif

    switch (pin)
    {
        case CONFIG_BLYNK_PIN_ROOM_TEMPERATURE:
            Blynk.virtualWrite(pin, m_lastRoomTemperature / 100.f);
            break;

        default:
            break;
    }
}

const BlynkHandler::BlynkEvent& BlynkHandler::blynkEvent() const
{
    return m_event;
}
