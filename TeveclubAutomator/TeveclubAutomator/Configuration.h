#pragma once

#include <QSettings>

#include "IConfiguration.h"

class Configuration : public IConfiguration
{
public:
    Configuration(const QString& filePath);

    QVariant readValue(const QString& key) const override;

private:
    QSettings m_settings;
};
