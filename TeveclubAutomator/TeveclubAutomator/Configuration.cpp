#include "Configuration.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(ConfigurationLog, "Configuration")

Configuration::Configuration(const QString& filePath)
    : m_settings{ filePath, QSettings::Format::IniFormat }
{
}

QVariant Configuration::readValue(const QString& key) const
{
    if (!m_settings.contains(key))
    {
        qCWarning(ConfigurationLog) << "value not found:" << key;
        return{};
    }

    return m_settings.value(key);
}

