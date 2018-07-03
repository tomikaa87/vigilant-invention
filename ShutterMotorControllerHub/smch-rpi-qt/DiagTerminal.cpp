/*
 * DiagTerminal.cpp
 *
 *  Created on: 2018. maj. 27.
 *      Author: tkarpati
 */

#include "DiagTerminal.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <QLoggingCategory>
#include <QTcpServer>
#include <QTcpSocket>

Q_LOGGING_CATEGORY(DiagTerminalLog, "DiagTerminal")

DiagTerminal::DiagTerminal(QObject *parent)
    : QObject(parent)
{
    initServer();
}

void DiagTerminal::printPrompt(const char* validOptions)
{
    QString s;

    s.append("Selection");

    if (validOptions)
    {
        s.append(QString{ " (%1)" }.arg(validOptions));
        mValidOptions = validOptions;
    }

    s.append(": ");

    sendDataOnSocket(s.toUtf8());
}

void DiagTerminal::printMenu()
{
    sendDataOnSocket("\r\n");

    switch (mState)
    {
        case State::InMainMenu:
            printMainMenu();
            break;

        case State::InSendCommandMenu:
            printSendCommandMenu();
            break;

        case State::InSelectDeviceMenu:
            printSelectDeviceMenu();
            break;

        default:
            break;
    }
}

void DiagTerminal::printMainMenu()
{
    QString s;

    s.append("++ SMR Hub Diagnostic Menu ++\r\n");
    s.append("1) Scan SMRR units\r\n");
    s.append("2) Select SMRR unit\r\n");
    s.append("3) Read status\r\n");
    s.append("4) Send command\r\n");
    s.append("x) Exit\r\n");

    sendDataOnSocket(s.toUtf8());

    printPrompt("1234x");
}

void DiagTerminal::printSendCommandMenu()
{
    QString s;

    s.append("++ Send Command Menu ++\r\n");
    s.append("1) Shutter 1 Up\r\n");
    s.append("2) Shutter 1 Down\r\n");
    s.append("3) Shutter 2 Up\r\n");
    s.append("4) Shutter 2 Down\r\n");
    s.append("5) Back\r\n");

    sendDataOnSocket(s.toUtf8());

    printPrompt("12345");
}

void DiagTerminal::printSelectDeviceMenu()
{
    QString s;

    s.append("++ Select Device Menu ++\r\n");
    s.append("0) SMRR0\r\n");
    s.append("1) SMRR1\r\n");
    s.append("2) SMRR2\r\n");
    s.append("3) SMRR3\r\n");
    s.append("4) SMRR4\r\n");
    s.append("5) SMRR5\r\n");
    s.append("6) SMRR6\r\n");
    s.append("7) SMRR7\r\n");
    s.append("8) SMRR8\r\n");
    s.append("9) SMRR9\r\n");
    s.append("b) Back\r\n");

    sendDataOnSocket(s.toUtf8());

    printPrompt("0123456789b");
}

uint8_t DiagTerminal::selectedDeviceIndex()
{
    return mSelectedDeviceIndex;
}

void DiagTerminal::initServer()
{
    if (m_server)
    {
        qCWarning(DiagTerminalLog) << "server is already initialized";
        return;
    }

    m_server = new QTcpServer(this);

    connect(m_server, &QTcpServer::newConnection, [this] {
        if (!m_server->hasPendingConnections())
        {
            qCWarning(DiagTerminalLog) << "new connection accepted, but no pending connections";
            return;
        }

        auto socket = m_server->nextPendingConnection();

        qCInfo(DiagTerminalLog) << "client connected:" << socket->localAddress().toString();

        if (m_socket)
        {
            qCWarning(DiagTerminalLog) << "only one client is supported";

            socket->write("Error: only one active client is supported\r\n");
            socket->flush();
            socket->close();
            socket->deleteLater();

            return;
        }

        m_socket = socket;

        printMenu();

        connect(socket, &QTcpSocket::readyRead, [this, socket] {
            onDataReceived(socket->readAll());
        });

        connect(socket, &QTcpSocket::disconnected, [this] {
            qCDebug(DiagTerminalLog) << "client disconnected";
            m_socket->deleteLater();
            m_socket = nullptr;
        });
    });

    if (!m_server->listen(QHostAddress::Any, 23456))
    {
        qCCritical(DiagTerminalLog) << "failed to start listening on port" << m_server->serverPort();
        return;
    }

    qCInfo(DiagTerminalLog) << "listening on port" << m_server->serverPort();
}

void DiagTerminal::onDataReceived(const QByteArray &data)
{
    if (data.isEmpty())
    {
        qCWarning(DiagTerminalLog) << "empty data buffer received";
        return;
    }

    char c = data[0];

    if (isalnum(c))
    {
        // Process incoming character as a choice
        sendDataOnSocket(QByteArray::fromRawData(&c, 1));

        if (mValidOptions && strchr(mValidOptions, c))
        {
            sendDataOnSocket("\r\n");
            selectOption(c);
        }
        else
        {
            sendDataOnSocket("\r\nInvalid option selected\r\n");
            printMenu();
        }
    }
    else if (c == '\r')
    {
        printMenu();
    }
}

void DiagTerminal::sendDataOnSocket(const QByteArray &data)
{
    if (!m_socket)
    {
        qCWarning(DiagTerminalLog) << "cannot send data on a null socket";
        return;
    }

    m_socket->write(data);
    m_socket->flush();
}

void DiagTerminal::selectOption(char option)
{
    Action action = Action::Nothing;

    switch (mState)
    {
        case State::InMainMenu:
        {
            switch (option)
            {
                case '1':
                    action = Action::ScanUnits;
                    break;

                case '2':
                    mState = State::InSelectDeviceMenu;
                    printMenu();
                    break;

                case '3':
                    sendDataOnSocket("Reading status\r\n");
                    action = Action::ReadStatus;
                    break;

                case '4':
                    mState = State::InSendCommandMenu;
                    printMenu();
                    break;

                case 'x':
                    if (m_socket)
                    {
                        m_socket->close();
                    }
                    mState = State::InMainMenu;
                    mMenuUpdateNeeded = false;
                    break;

                default:
                    break;
            }

            break;
        }

        case State::InSendCommandMenu:
        {
            switch (option)
            {
                case '1':
                    action = Action::Shutter1Up;
                    break;

                case '2':
                    action = Action::Shutter1Down;
                    break;

                case '3':
                    action = Action::Shutter2Up;
                    break;

                case '4':
                    action = Action::Shutter2Down;
                    break;

                case '5':
                    mState = State::InMainMenu;
                    printMenu();
                    break;
            }

            break;
        }

        case State::InSelectDeviceMenu:
        {
            if (option >= '0' && option <= '9')
            {
                action = Action::SelectDevice;
                mSelectedDeviceIndex = option - '0';
                mState = State::InMainMenu;
                mMenuUpdateNeeded = true;
            }
            else if (option == 'b')
            {
                mState = State::InMainMenu;
                mMenuUpdateNeeded = true;
            }

            break;
        }

        default:
            break;
    }

    if (action != Action::Nothing)
    {
        emit actionTriggered(action);
    }

    if (mMenuUpdateNeeded)
    {
        mMenuUpdateNeeded = false;
        printMenu();
    }
}
