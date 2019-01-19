#include "QtServerAdapter.h"

#include <algorithm>
#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(QtServerAdapterLog)
Q_LOGGING_CATEGORY(QtServerAdapterLog, "QtServerAdapter")

QtServerAdapter::QtServerAdapter(QObject *parent)
    : QObject{ parent }
    , m_server{ new QTcpServer{ this } }
{
    connect(m_server, &QTcpServer::newConnection, this, &QtServerAdapter::onServerNewConnection);
}

bool QtServerAdapter::startListening()
{
    if (m_server->isListening())
        return false;

    qCDebug(QtServerAdapterLog) << "starting listening on port 50000";

    return m_server->listen(QHostAddress::Any, 50000);
}

void QtServerAdapter::onClientConnected(ClientConnectedHandler&& handler)
{
    m_clientConnectedHandler = std::move(handler);
}

void QtServerAdapter::onClientDisconnected(ClientDisconnectedHandler&& handler)
{
    m_clientDisconnectedHandler = std::move(handler);
}

void QtServerAdapter::onClientDataReceived(ClientDataReceivedHandler&& handler)
{
    m_clientDataReceivedHandler = std::move(handler);
}

size_t QtServerAdapter::availableDataBytes(const uint16_t endpointId) const
{
    auto&& it = m_sockets.find(endpointId);
    if (it == m_sockets.end())
        return 0;

    return it->second->bytesAvailable();
}

size_t QtServerAdapter::sendDataToClient(const uint16_t endpointId, const char* const data, const size_t length)
{
    auto&& it = m_sockets.find(endpointId);
    if (it == m_sockets.end())
        return 0;

    return it->second->write(data, length);
}

size_t QtServerAdapter::readClientData(const uint16_t endpointId, char* data, const size_t length)
{
    auto&& it = m_sockets.find(endpointId);
    if (it == m_sockets.end())
        return 0;

    return it->second->read(data, length);
}

void QtServerAdapter::onServerNewConnection()
{
    while (m_server->hasPendingConnections())
    {
        acceptIncomingConnection(m_server->nextPendingConnection());
    }
}

void QtServerAdapter::acceptIncomingConnection(QTcpSocket* socket)
{
    if (!socket)
        return;

    auto id = m_nextEndpointId++;

    m_sockets.emplace(id, socket);
    m_endpointIds.emplace(socket, id);

    qCDebug(QtServerAdapterLog).nospace().noquote() << "(" << toString(socket) << ") incoming connection";

    connect(socket, &QTcpSocket::stateChanged, this, &QtServerAdapter::onClientStateChanged);
    connect(socket, &QTcpSocket::readyRead, this, &QtServerAdapter::onClientReadyRead);

    if (m_clientConnectedHandler)
        m_clientConnectedHandler(id);
}

void QtServerAdapter::onClientStateChanged(const QAbstractSocket::SocketState state)
{
    auto socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket)
        return;

    qCDebug(QtServerAdapterLog).nospace().noquote() << "(" << toString(socket) << ") state: " << state;

    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
    {
        qCDebug(QtServerAdapterLog).nospace().noquote() << "(" << toString(socket) << ") destroying";

        auto id = m_endpointIds[socket];

        if (m_clientDisconnectedHandler)
            m_clientDisconnectedHandler(id);

        m_sockets.erase(id);
        m_endpointIds.erase(socket);

        socket->deleteLater();
        break;
    }

    default:
        break;
    }
}

void QtServerAdapter::onClientReadyRead()
{
    auto socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket)
        return;

    qCDebug(QtServerAdapterLog).nospace().noquote() << "(" << toString(socket) << ") data received, available: " << socket->bytesAvailable() << " B";

    if (!m_clientDataReceivedHandler)
        return;

    m_clientDataReceivedHandler(m_endpointIds[socket]);
}

QString QtServerAdapter::toString(const QTcpSocket* socket) const
{
    return QString{ "[%1] %2:%3" }.arg(m_endpointIds.at(socket)).arg(socket->peerAddress().toString()).arg(socket->peerPort());
}
