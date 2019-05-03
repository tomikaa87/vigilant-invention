#pragma once
#include <QVariant>

class IConfiguration
{
public:
    virtual ~IConfiguration() = default;

    virtual QVariant readValue(const QString& key) const = 0;
};
