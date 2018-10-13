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
#include "IHub.h"
#include "ScheduleDetails.h"

#include <QObject>
#include <QElapsedTimer>

#include <cstdint>
#include <unordered_map>

class TimeInputParam;

class Hub: public QObject, public IHub
{
    Q_OBJECT

public:
    Hub(QObject* parent = nullptr);

    void task();

    void readStatus(std::function<void(RemoteDeviceStatus&& status)>&& callback = {}) override;
    void shutter1Up() override;
    void shutter1Down() override;
    void shutter2Up() override;
    void shutter2Down() override;

    void scanUnits(std::function<void()>&& callback = {}) override;

    const std::unordered_map<uint8_t, RemoteDevice>& devices() const override;

    void selectDevice(uint8_t index) override;

    void setSchedulerEnabled(bool enabled);
    void setSchedule(ScheduleDetails&& sch);

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

    struct ScannedDevice
    {
        RemoteDevice remoteDevice;
        bool active = false;
    };

    std::unordered_map<uint8_t, RemoteDevice> mDevices;

    struct Schedule
    {
        ScheduleDetails details;
        bool enabled = false;
        bool lastCheckResult = false;
        QElapsedTimer timer;

        bool check() const;
    } mSchedule;

    ScanState mScanState = ScanState::Idle;
    uint8_t mScanDeviceIndex = 0;
    uint8_t mScanRetryCount = 0;
    QElapsedTimer mTimer;
    ScannedDevice mScannedDevice;
//    bool mActiveDevices[10] = { 0 };
    uint8_t mSelectedDeviceIndex = 0;

    std::function<void()> mScanCallback;
    std::function<void(RemoteDeviceStatus&& status)> mReadStatusCallback;

    hub::BlynkTransportSocket mBlynkTransport;
    hub::BlynkSocket mBlynk;

    void sendMessage(protocol_msg_t* msg);

    void progressScanning(Radio::TaskResult radioTaskResult);
    void progressSendCommand(Radio::TaskResult radioTaskResult);

    void processStatusResponse(const protocol_msg_t& msg);

    void onScheduleStateChanged(bool on);
};

#endif /* HUB_HUB_H_ */
