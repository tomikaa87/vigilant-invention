#include "PersistentStorage.h"
#include "Config.h"

#include <Arduino.h>
#include <Crc16.h>
#include <EEPROM.h>

PersistentStorage::PersistentStorage()
{
    Serial.println("PersistentStorage: setting up EEPROM");

    EEPROM.begin(sizeof(uint16_t) + sizeof(PersistentConfig));
}

void PersistentStorage::loadConfiguration()
{
    Serial.println("PersistentStorage: loading configuration");

    uint16_t storedChecksum;

    EEPROM.get(Config::Persistent::BaseAddress, storedChecksum);
    EEPROM.get(Config::Persistent::BaseAddress + sizeof(storedChecksum), config);

    uint16_t calculatedChecksum = calculateConfigChecksum();

    Serial.printf("PersistentStorage: stored checksum: 0x%04X, calculated checksum: 0x%04X\r\n",
                  storedChecksum, calculatedChecksum);

    if (storedChecksum != calculatedChecksum)
    {
        Serial.println("PersistentStorage: checksum of loaded configuration doesn't match");
        loadDefaults();
    }

    m_lastChecksum = calculateConfigChecksum();

    Serial.printf("PersistentStorage: checksum after loading: 0x%04X\r\n", m_lastChecksum);
}

void PersistentStorage::saveConfiguration()
{
    Serial.println("PersistentStorage: saving configuration");

    auto checksum = calculateConfigChecksum();

    Serial.printf("PersistentStorage: calculated checksum: 0x%04X\r\n", checksum);

    if (checksum == m_lastChecksum)
    {
        Serial.println("PersistentStorage: last checksum matches, no need to commit");
        return;
    }

    EEPROM.put(Config::Persistent::BaseAddress, checksum);
    EEPROM.put(Config::Persistent::BaseAddress + sizeof(checksum), config);
    EEPROM.commit();

    // Just to check if everything went fine
    loadConfiguration();
}

void PersistentStorage::loadDefaults()
{
    Serial.println("PersistentStorage: loading default configuration");

    config = PersistentConfig{};
}

uint16_t PersistentStorage::calculateConfigChecksum()
{
    Crc16 crc;

    auto configData = reinterpret_cast<uint8_t*>(&config);
    for (int i = 0; i < sizeof(config); ++i)
        crc.updateCrc(configData[i]);

    return crc.getCrc();
}
