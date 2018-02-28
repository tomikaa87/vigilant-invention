#include "PersistentStorage.h"
#include "Config.h"

#include <Crc16.h>
#include <EEPROM.h>

PersistentStorage::PersistentStorage()
    : m_log{ "PersistentStorage" }
{
    m_log.debug("Setting up EEPROM");

    EEPROM.begin(sizeof(uint16_t) + sizeof(PersistentConfig));
}

void PersistentStorage::loadConfiguration()
{
    m_log.info("Loading configuration");

    uint16_t storedChecksum;

    EEPROM.get(Config::Persistent::BaseAddress, storedChecksum);
    EEPROM.get(Config::Persistent::BaseAddress + sizeof(storedChecksum), config);

    uint16_t calculatedChecksum = calculateConfigChecksum();

    m_log.debug("Stored checksum: 0x%04X, calculated checksum: 0x%04X",
                storedChecksum, calculatedChecksum);

    if (storedChecksum != calculatedChecksum)
    {
        m_log.warning("Checksum of loaded configuration doesn't match");
        loadDefaults();
    }

    m_lastChecksum = calculateConfigChecksum();

    m_log.debug("Checksum after loading: 0x%04X", m_lastChecksum);
}

void PersistentStorage::saveConfiguration()
{
    m_log.info("Saving configuration");

    auto checksum = calculateConfigChecksum();

    m_log.debug("Calculated checksum: 0x%04X", checksum);

    if (checksum == m_lastChecksum)
    {
        m_log.debug("Last checksum matches, no need to commit");
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
    m_log.warning("Loading default configuration");

    config = PersistentConfig{};
}

uint16_t PersistentStorage::calculateConfigChecksum() const
{
    Crc16 crc;

    auto configData = reinterpret_cast<const uint8_t*>(&config);
    for (int i = 0; i < sizeof(config); ++i)
        crc.updateCrc(configData[i]);

    return crc.getCrc();
}
