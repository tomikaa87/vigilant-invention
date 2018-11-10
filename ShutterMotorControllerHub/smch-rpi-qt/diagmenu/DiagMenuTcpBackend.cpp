#include "DiagMenuTcpBackend.h"

#include <QDebug>
#include <QLoggingCategory>
#include <QTcpServer>
#include <QTcpSocket>

Q_DECLARE_LOGGING_CATEGORY(DiagMenuTcpBackendLog)
Q_LOGGING_CATEGORY(DiagMenuTcpBackendLog, "DiagMenuTcp")

DiagMenuTcpBackend::DiagMenuTcpBackend(const QHostAddress& listenAddress, uint16_t listenPort, QObject* parent)
    : AbstractDiagMenuBackend(parent)
    , m_server(new QTcpServer(this))
    , m_listenAddress(listenAddress)
    , m_listenPort(listenPort)
{
    m_server->setMaxPendingConnections(1);
    connect(m_server, &QTcpServer::newConnection, this, &DiagMenuTcpBackend::onNewConnection);
}

void DiagMenuTcpBackend::startListening()
{
    if (!m_server->listen(m_listenAddress, m_listenPort))
    {
        qCWarning(DiagMenuTcpBackendLog) << "cannot start listening on TCP port" << m_listenPort;
        qCWarning(DiagMenuTcpBackendLog) << "error:" << m_server->errorString();
    }
}

void DiagMenuTcpBackend::send(const QByteArray& data)
{
    if (!m_socket || !m_socket->isOpen())
        return;

    m_socket->write(data);
    m_socket->flush();
}

void DiagMenuTcpBackend::disconnectClient()
{
    if (m_socket)
    {
        m_socket->close();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

void DiagMenuTcpBackend::onNewConnection()
{
    if (!m_server->hasPendingConnections())
        return;

    auto socket = m_server->nextPendingConnection();

    if (!m_socket)
    {
        m_socket = socket;

        connect(m_socket, &QTcpSocket::stateChanged, [this](QAbstractSocket::SocketState s) {
            if (!m_socket)
                return;

            if (s == QAbstractSocket::UnconnectedState)
            {
                m_socket->close();
                m_socket->deleteLater();
                m_socket = nullptr;
            }
        });

        connect(m_socket, &QTcpSocket::readyRead, [this] {
            if (!m_socket)
                return;

            char c;
            while (m_socket && m_socket->bytesAvailable() > 0)
            {
                m_socket->read(&c, 1);
                emit received(c);
            }
        });

        emit connected();

        return;
    }

    socket->write("Diag Menu is already connected\r\n");

    socket->close();
    socket->deleteLater();
}
