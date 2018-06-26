/*
 * Hub.h
 *
 *  Created on: 2018. máj. 26.
 *      Author: tomikaa
 */

#ifndef HUB_HUB_H_
#define HUB_HUB_H_

#include <hub/Radio.h>
#include <hub/ElapsedTimer.h>
#include <stdint.h>

class Hub
{
public:
    Hub();

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
    ElapsedTimer mTimer;
    bool mActiveDevices[10] = { 0 };
    uint8_t mSelectedDeviceIndex = 0;

    void sendMessage(protocol_msg_t* msg);

    void progressScanning(Radio::TaskResult radioTaskResult);
    void progressSendCommand(Radio::TaskResult radioTaskResult);
};

#endif /* HUB_HUB_H_ */
