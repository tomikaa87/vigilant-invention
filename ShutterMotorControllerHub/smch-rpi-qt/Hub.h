/*
 * Hub.h
 *
 *  Created on: 2018. maj. 26.
 *      Author: tomikaa
 */

#ifndef HUB_HUB_H_
#define HUB_HUB_H_

#include "Radio.h"
#include "BlynkSocket.h"

#include <QObject>
#include <QElapsedTimer>

#include <stdint.h>

class Hub: public QObject
{
    Q_OBJECT

public:
    Hub(QObject* parent = nullptr);

    void task();

    void readStatus();
    void shutter1Up();
    void shutter1Down();
    void shutter2Up();
    void shutter2Down();

    void scanUnits();

    void selectDevice(uint8_t index);

private:
    Radio mRadio;

    enum class State
    {
        Idle,
        Scanning,
        SendingMessage
    };

    State mState = State::Idle;

    enum class ScanState
    {
        Idle,
        Starting,
        SendStatusRequest,
        WaitingForStatusPacket,
        StatusRequestPacketLost,
        ProcessStatusPacket
    };

    ScanState mScanState = ScanState::Idle;
    uint8_t mScanDeviceIndex = 0;
    uint8_t mScanRetryCount = 0;
    QElapsedTimer mTimer;
    bool mActiveDevices[10] = { 0 };
    uint8_t mSelectedDeviceIndex = 0;

    hub::BlynkTransportSocket mBlynkTransport;
    hub::BlynkSocket mBlynk;

    void sendMessage(protocol_msg_t* msg);

    void progressScanning(Radio::TaskResult radioTaskResult);
    void progressSendCommand(Radio::TaskResult radioTaskResult);

    void processStatusResponse(const protocol_msg_t& msg);
};

#endif /* HUB_HUB_H_ */
