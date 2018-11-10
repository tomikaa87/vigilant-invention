#pragma once

#include <QObject>

class AbstractDiagMenuBackend : public QObject
{
    Q_OBJECT

public:
    explicit AbstractDiagMenuBackend(QObject* parent = nullptr) : QObject(parent) {}

    virtual void send(const QByteArray& data) = 0;

signals:
    void received(const char c);
};
