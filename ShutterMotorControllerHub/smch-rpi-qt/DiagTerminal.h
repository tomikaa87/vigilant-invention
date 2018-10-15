/*
 * DiagTerminal.h
 *
 *  Created on: 2018. maj. 27.
 *      Author: tkarpati
 */

#pragma once

#include <QObject>
#include <cstdint>
#include <memory>

#include "hub/IHub.h"

class QTcpServer;
class QTcpSocket;

class DiagTerminal: public QObject
{
    Q_OBJECT

public:
    DiagTerminal(std::shared_ptr<hub::IHub> hub,
                 QObject* parent = nullptr);

    enum class Action
    {
        Nothing,

        ReadStatus,

        Shutter1Up,
        Shutter1Down,
        Shutter2Up,
        Shutter2Down,

        ScanUnits,
        SelectDevice
    };

private:
    enum class State
    {
        InMainMenu,
        InSendCommandMenu,
        InSelectDeviceMenu
    };

    const std::shared_ptr<hub::IHub> m_hub;
    Action mAction = Action::Nothing;
    State mState = State::InMainMenu;
    bool mMenuUpdateNeeded = false;
    bool mUSARTInterruptRequest = false;
    const char* mValidOptions = nullptr;
    uint8_t mSelectedDeviceIndex = 0;

    QTcpServer* m_server = nullptr;
    QTcpSocket* m_socket = nullptr;

    void initServer();

    void onDataReceived(const QByteArray& data);
    void sendDataOnSocket(const QByteArray& data);

    void printMenu();
    void printPrompt(const char* validOptions = nullptr);
    void printMainMenu();
    void printSendCommandMenu();
    void printSelectDeviceMenu();

    void selectOption(char option);
};
