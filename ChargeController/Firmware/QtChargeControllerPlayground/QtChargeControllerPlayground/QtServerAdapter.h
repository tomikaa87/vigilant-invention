#pragma once

#include "IServerAdapter.h"

#include <list>
#include <map>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class QtServerAdapter : public QObject, public IServerAdapter
{
    Q_OBJECT

public:
    QtServerAdapter(QObject *parent = nullptr);

    bool startListening();

    // Inherited via INetworkAdapter
    virtual void onClientConnected(ClientConnectedHandler&& handler) override;
    virtual void onClientDisconnected(ClientDisconnectedHandler&& handler) override;
    virtual void onClientDataReceived(ClientDataReceivedHandler&& handler) override;
    virtual size_t availableDataBytes(const uint16_t endpointId) const override;
    virtual size_t sendDataToClient(const uint16_t endpointId, const char* const data, const size_t length) override;
    virtual size_t readClientData(const uint16_t endpointId, char* data, const size_t length) override;

private:
    IServerAdapter::ClientConnectedHandler m_clientConnectedHandler;
    IServerAdapter::ClientDisconnectedHandler m_clientDisconnectedHandler;
    IServerAdapter::ClientDataReceivedHandler m_clientDataReceivedHandler;

    std::map<uint16_t, QTcpSocket*> m_sockets;
    std::map<const QTcpSocket*, uint16_t> m_endpointIds;
    uint16_t m_nextEndpointId = 0;
    QTcpServer* m_server = nullptr;

    void onServerNewConnection();

    void acceptIncomingConnection(QTcpSocket* socket);

    void onClientStateChanged(const QAbstractSocket::SocketState state);
    void onClientReadyRead();

    QString toString(const QTcpSocket* socket) const;
};
