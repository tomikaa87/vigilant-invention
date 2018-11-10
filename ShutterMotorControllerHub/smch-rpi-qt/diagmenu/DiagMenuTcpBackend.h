#pragma once

#include "AbstractDiagMenuBackend.h"

#include <QHostAddress>

class QTcpServer;
class QTcpSocket;

class DiagMenuTcpBackend : public AbstractDiagMenuBackend
{
    Q_OBJECT

public:
    DiagMenuTcpBackend(const QHostAddress& listenAddress, uint16_t listenPort, QObject* parent = nullptr);

    void startListening();
    void stopListening();

    void send(const QByteArray& data) override;

    void disconnectClient();

signals:
    void connected();

private:
    QTcpServer* m_server;
    QTcpSocket* m_socket = nullptr;
    QHostAddress m_listenAddress;
    uint16_t m_listenPort;

    void onNewConnection();
};
