#include "BlynkHandler.h"
#include "PersistentStorage.h"
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

HANDLE_BLYNK_READ(CONFIG_BLYNK_PIN_OPEN_TIME_INPUT)
HANDLE_BLYNK_WRITE(CONFIG_BLYNK_PIN_OPEN_TIME_INPUT)

HANDLE_BLYNK_READ(CONFIG_BLYNK_PIN_SHUTTER_TIMER_ACTIVE_BUTTON)
HANDLE_BLYNK_WRITE(CONFIG_BLYNK_PIN_SHUTTER_TIMER_ACTIVE_BUTTON)

BlynkHandler::BlynkHandler(PersistentStorage& persistentStorage)
    : m_log{ "BlynkHandler" }
    , m_peristentStorage(persistentStorage)
{
    g_instance = this;

    m_log.debug("Starting Blynk");

    Blynk.begin(PrivateConfig::BlynkApiToken, PrivateConfig::WiFiSSID, PrivateConfig::WiFiPassword);

    m_log.debug("Blynk started");
    m_log.debug("Configuring WiFi");

    WiFi.setPhyMode(WIFI_PHY_MODE_11N);

    m_log.debug("WiFi configured");
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
    m_log.debug("Updating temperature: %d", value);
#endif

    m_lastRoomTemperature = value;
    updateVirtualPin(CONFIG_BLYNK_PIN_ROOM_TEMPERATURE);
}

void BlynkHandler::onConnected()
{
    m_log.debug("Connected to Blynk");

    updateVirtualPin(CONFIG_BLYNK_PIN_OPEN_TIME_INPUT);
    updateVirtualPin(CONFIG_BLYNK_PIN_SHUTTER_TIMER_ACTIVE_BUTTON);
}

void BlynkHandler::onButtonPressed(const int pin)
{
#ifdef DEBUG_BLYNK_HANDLER
    m_log.debug("Button pressed: %d", pin);
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
    m_log.debug("Virtual pin updated: %d to %s", pin, param.asString());
#endif

    switch (pin)
    {
        case CONFIG_BLYNK_PIN_OPEN_TIME_INPUT:
            handleOpenTimeInputChange(param);
            break;

        case CONFIG_BLYNK_PIN_SHUTTER_TIMER_ACTIVE_BUTTON:
            handleShutterTimerActiveChange(param);
            break;

        default:
            break;
    }
}

void BlynkHandler::updateVirtualPin(const int pin)
{
#ifdef DEBUG_BLYNK_HANDLER
    m_log.debug("Virtual pin update requested: %d", pin);
#endif

    switch (pin)
    {
        case CONFIG_BLYNK_PIN_ROOM_TEMPERATURE:
            Blynk.virtualWrite(pin, m_lastRoomTemperature / 100.f);
            break;

        case CONFIG_BLYNK_PIN_OPEN_TIME_INPUT:
            updateOpenTimeInput();
            break;

        case CONFIG_BLYNK_PIN_SHUTTER_TIMER_ACTIVE_BUTTON:
            Blynk.virtualWrite(pin, m_peristentStorage.config.ShutterTimerActive ? 1 : 0);
            break;

        default:
            break;
    }
}

void BlynkHandler::addVirtualTerminalLogLine(const char* line) const
{
    WidgetTerminal terminal{ CONFIG_BLYNK_PIN_VIRTUAL_TERMINAL };
    terminal.write(line);
    terminal.flush();
}

const BlynkHandler::BlynkEvent& BlynkHandler::blynkEvent() const
{
    return m_event;
}

void BlynkHandler::handleOpenTimeInputChange(const BlynkParam& param)
{
    TimeInputParam tip{ param };

    if (!tip.hasStartTime())
    {
        m_log.warning("Shutter open time input has no start time");
        return;
    }

    if (!tip.hasStopTime())
    {
        m_log.warning("Shutter open time input has no stop time");
        return;
    }

    auto& cfg = m_peristentStorage.config;

    cfg.ShutterOpenHour = tip.getStartHour();
    cfg.ShutterOpenMinute = tip.getStartMinute();
    cfg.ShutterCloseHour = tip.getStopHour();
    cfg.ShutterCloseMinute = tip.getStopMinute();

    m_log.info("Shutter open time updated. From: %02d:%02d, to: %02d:%02d",
               cfg.ShutterOpenHour,
               cfg.ShutterOpenMinute,
               cfg.ShutterCloseHour,
               cfg.ShutterCloseMinute);

    m_peristentStorage.saveConfiguration();
}

void BlynkHandler::updateOpenTimeInput()
{
    const auto& cfg = m_peristentStorage.config;

    const uint32_t startAt = cfg.ShutterOpenHour * 3600 + cfg.ShutterOpenMinute * 60;
    const uint32_t stopAt = cfg.ShutterCloseHour * 3600 + cfg.ShutterCloseMinute * 60;

    Blynk.virtualWrite(CONFIG_BLYNK_PIN_OPEN_TIME_INPUT, startAt, stopAt);
}

void BlynkHandler::handleShutterTimerActiveChange(const BlynkParam & param)
{
    m_log.info("Shutter timer state changed to: %s",
               param.asInt() > 0 ? "active" : "inactive");

    m_peristentStorage.config.ShutterTimerActive = param.asInt() > 0 ? true : false;

    m_peristentStorage.saveConfiguration();
}
