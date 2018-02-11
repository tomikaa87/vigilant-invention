#include "TemperatureSensor.h"
#include "Config.h"

TemperatureSensor::TemperatureSensor()
    : m_oneWire(Config::Pins::OneWire)
{
    static_assert(Config::DS18B20Resolution <= 12, "DS18B20 resolution must be at most 12");
    static_assert(Config::DS18B20Resolution >= 9, "DS18B20 resolution must be at least 9");
}

void TemperatureSensor::task()
{
    auto currentTime = ::millis();

    if (currentTime - m_lastReadTime < Config::TemperatureSensorReadInterval)
        return;

    m_lastReadTime = currentTime;

    auto value = read();
    if (value != m_lastValue)
    {
        m_lastValue = value;
        m_temperatureChangedEvent(value);
    }
}

int16_t TemperatureSensor::temperature() const
{
    return m_lastValue;
}

const TemperatureSensor::TemperatureChangedEvent& TemperatureSensor::temperatureChangedEvent() const
{
    return m_temperatureChangedEvent;
}

int16_t TemperatureSensor::read()
{
#ifdef DEBUG_TEMPERATURE_SENSOR
    Serial.printf("DS18B20 read:\r\n");
#endif

    m_oneWire.reset();

    m_oneWire.write(0xCC);
    m_oneWire.write(0x44);

    ::delayMicroseconds(200);

    m_oneWire.reset();

    m_oneWire.write(0xCC);
    m_oneWire.write(0xBE);

    uint16_t value = m_oneWire.read();
    value += m_oneWire.read() << 8;

    if (value & 0x8000)
        value = ~value + 1;

#ifdef DEBUG_TEMPERATURE_SENSOR
    Serial.printf("  raw value: %04Xh\r\n", value);
#endif

    int16_t temperature = 0;

    // Calculate integer part
    temperature = (value >> (Config::DS18B20Resolution - 8)) * 100;

#ifdef DEBUG_TEMPERATURE_SENSOR
    Serial.printf("  integer part: %d\r\n", temperature);
#endif

    // Calculate fractional part
    temperature += (((value << (4 - (Config::DS18B20Resolution - 8))) & 0xF) * 625) / 100;

    if (value & 0x8000)
        temperature *= -1;

#ifdef DEBUG_TEMPERATURE_SENSOR
    Serial.printf("  value: %d\r\n", temperature);
#endif

    return temperature;
}
