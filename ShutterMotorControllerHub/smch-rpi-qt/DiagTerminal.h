/*
 * DiagTerminal.h
 *
 *  Created on: 2018. maj. 27.
 *      Author: tkarpati
 */

#ifndef HUB_DIAGTERMINAL_H_
#define HUB_DIAGTERMINAL_H_

#include <QObject>
#include <cstdint>

class QTcpServer;
class QTcpSocket;

class DiagTerminal: public QObject
{
    Q_OBJECT

public:
    DiagTerminal(QObject* parent = nullptr);

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

    uint8_t selectedDeviceIndex();

signals:
    void actionTriggered(Action action);

private:
    enum class State
    {
        InMainMenu,
        InSendCommandMenu,
        InSelectDeviceMenu
    };

    Action mAction = Action::Nothing;
    State mState = State::InMainMenu;
    bool mMenuUpdateNeeded = true;
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

#endif /* HUB_DIAGTERMINAL_H_ */
