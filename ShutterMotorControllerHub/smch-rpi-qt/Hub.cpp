/*
 * Hub.cpp
 *
 *  Created on: 2018. maj. 26.
 *      Author: tomikaa
 */

#include "Hub.h"

#include "BlynkSocket.h"
#include "PrivateConfig.h"

#include <QLoggingCategory>
Q_LOGGING_CATEGORY(HubLog, "Hub")

// FIXME
static Hub* s_hub = nullptr;

Hub::Hub(QObject* parent)
    : QObject(parent)
    , mBlynk(mBlynkTransport)
{
    s_hub = this;
    mBlynk.begin(BlynkToken);
}

void Hub::task()
{
    mBlynk.run();

    auto result = mRadio.task();

    switch (result)
    {
        case Radio::TaskResult::Busy:
//            qCDebug(HubLog) << "Radio is busy";
            break;

        case Radio::TaskResult::PacketLost:
            qCDebug(HubLog) << "Packet lost";
            break;

        case Radio::TaskResult::DataSent:
            qCDebug(HubLog) << "Data sent";
            break;

        case Radio::TaskResult::DataReceived:
            qCDebug(HubLog) << "Data received";
            break;

        case Radio::TaskResult::None:
            break;
    }

    switch (mState)
    {
        case State::Scanning:
            progressScanning(result);
            break;

        case State::SendingMessage:
            progressSendCommand(result);
            break;

        case State::Idle:
            break;
    }
}

void Hub::sendMessage(protocol_msg_t* msg)
{
    if (mState != State::Idle)
    {
        qCInfo(HubLog, "Cannot send message to device %u while Hub is busy", mSelectedDeviceIndex);
        return;
    }

    qCInfo(HubLog, "Sending message to device %u", mSelectedDeviceIndex);

    char address[] = "SMRR0";
    address[4] = '0' + mSelectedDeviceIndex;

    mRadio.sendMessage(reinterpret_cast<const uint8_t*>(address), msg);

    mState = State::SendingMessage;
    mTimer.start();
}

void Hub::readStatus(std::function<void (RemoteDeviceStatus&& status)>&& callback)
{
    protocol_msg_t msg;
    protocol_msg_init(&msg);
    msg.msg_type = PROTO_MSG_READ_STATUS;

    mReadStatusCallback = std::move(callback);

    sendMessage(&msg);
}

void Hub::shutter1Up()
{
    protocol_msg_t msg;
    protocol_msg_init(&msg);
    msg.msg_type = PROTO_MSG_COMMAND;
    msg.payload.command = PROTO_CMD_SHUTTER_1_UP;

    sendMessage(&msg);
}

void Hub::shutter1Down()
{
    protocol_msg_t msg;
    protocol_msg_init(&msg);
    msg.msg_type = PROTO_MSG_COMMAND;
    msg.payload.command = PROTO_CMD_SHUTTER_1_DOWN;

    sendMessage(&msg);
}

void Hub::shutter2Up()
{
    protocol_msg_t msg;
    protocol_msg_init(&msg);
    msg.msg_type = PROTO_MSG_COMMAND;
    msg.payload.command = PROTO_CMD_SHUTTER_2_UP;

    sendMessage(&msg);
}

void Hub::shutter2Down()
{
    protocol_msg_t msg;
    protocol_msg_init(&msg);
    msg.msg_type = PROTO_MSG_COMMAND;
    msg.payload.command = PROTO_CMD_SHUTTER_2_DOWN;

    sendMessage(&msg);
}

void Hub::scanUnits(std::function<void()>&& callback)
{
    if (mState != State::Idle)
    {
        qCInfo(HubLog) << "Cannot initiate scan while Hub is busy";
        return;
    }

    qCInfo(HubLog) << "Scanning SMRR units";

    mState = State::Scanning;
    mScanState = ScanState::Starting;
    mScanCallback = std::move(callback);
}

const std::unordered_map<uint8_t, IHub::RemoteDevice>& Hub::devices() const
{
    return mDevices;
}

void Hub::progressScanning(Radio::TaskResult radioTaskResult)
{
    switch (mScanState)
    {
        case ScanState::Idle:
            break;

        case ScanState::Starting:
        {
            qCDebug(HubLog) << "Starting scan";
            mScanDeviceIndex = 0;
            mScanState = ScanState::SendStatusRequest;
            mDevices.clear();
//            mScannedDevice = {};
            break;
        }

        case ScanState::SendStatusRequest:
        {
            if (mScanRetryCount >= 3)
            {
                qCDebug(HubLog) << "Max retry count reached";
//                mActiveDevices[mScanDeviceIndex] = false;
                ++mScanDeviceIndex;
                mScanRetryCount = 0;
            }

            if (mScanDeviceIndex >= 10)
            {
                qCDebug(HubLog) << "Max device index reached";
                mScanState = ScanState::Idle;
                mState = State::Idle;

                qCInfo(HubLog) << "Scan finished";
                qCInfo(HubLog) << "Active devices:";
                for (const auto& d : mDevices)
                {
                    qCInfo(HubLog).nospace()
                            << "  "
                            << d.first << ": "
                            << d.second.name.c_str()
                            << " (Firmware: " << d.second.firmwareVersion.c_str()
                            << ")";
                }
//                for (uint8_t i = 0; i < sizeof(mActiveDevices); ++i)
//                {
//                    if (!mActiveDevices[i])
//                        continue;

//                    qCInfo(HubLog, "  SMRR%c", '0' + i);
//                }

                if (mScanCallback)
                {
                    mScanCallback();
                    mScanCallback = {};
                }

                break;
            }

            protocol_msg_t msg;
            protocol_msg_init(&msg);
            msg.msg_type = PROTO_MSG_READ_STATUS;

            char address[] = "SMRR0";
            address[4] = '0' + mScanDeviceIndex;

            qCDebug(HubLog, "Requesting status from %s", address);

            mRadio.sendMessage(reinterpret_cast<const uint8_t*>(address), &msg);

            mScanState = ScanState::WaitingForStatusPacket;
            mTimer.start();
            break;
        }

        case ScanState::WaitingForStatusPacket:
        {
            switch (radioTaskResult)
            {
                case Radio::TaskResult::DataReceived:
                    mScanState = ScanState::ProcessStatusPacket;
                    break;

                case Radio::TaskResult::PacketLost:
                    mScanState = ScanState::StatusRequestPacketLost;
                    break;

                case Radio::TaskResult::DataSent:
                    break;

                default:
                    break;
            }

            if (mTimer.elapsed() > 200)
            {
                qCDebug(HubLog) << "Response timeout";
                mScanState = ScanState::StatusRequestPacketLost;
            }

            break;
        }

        case ScanState::StatusRequestPacketLost:
        {
            qCDebug(HubLog) << "Status request packet lost";
            ++mScanRetryCount;
            mScanState = ScanState::SendStatusRequest;
            break;
        }

        case ScanState::ProcessStatusPacket:
        {
            qCDebug(HubLog) << "Device found";
//            mActiveDevices[mScanDeviceIndex] = true;
//            mScannedDevice.active = true;

            RemoteDevice device;
            device.name = std::string{ "SMRR" } + char(mScanDeviceIndex + '0');

            auto&& msg = mRadio.lastReceivedMessage();

            if (msg.msg_type == PROTO_MSG_READ_STATUS_RESULT)
            {
                device.firmwareVersion = std::string{ msg.payload.status.firmware_ver };
            }
            else
            {
                qCWarning(HubLog) << "scan: last message is not status result";
            }

            mDevices.emplace(mScanDeviceIndex, std::move(device));

            ++mScanDeviceIndex;
            mScanRetryCount = 0;
            mScanState = ScanState::SendStatusRequest;
            break;
        }
    }
}

void Hub::progressSendCommand(Radio::TaskResult radioTaskResult)
{
    switch (radioTaskResult)
    {
        case Radio::TaskResult::DataReceived:
        {
            auto&& msg = mRadio.lastReceivedMessage();

            switch (msg.msg_type)
            {
                case PROTO_MSG_READ_STATUS_RESULT:
                    processStatusResponse(msg);
                    break;

                default:
                    qCInfo(HubLog) << "Unprocessed response:" << msg.msg_type;
            }

            break;
        }

        case Radio::TaskResult::PacketLost:
        {
            qCInfo(HubLog) << "Status request packet lost";
            mState = State::Idle;
        }
        [[clang::fallthrough]];

        default:
        {
            if (mTimer.elapsed() > 200)
            {
                qCInfo(HubLog) << "Status request timed out";
                mState = State::Idle;
            }

            break;
        }
    }
}

void Hub::selectDevice(uint8_t index)
{
    if (index > 9)
    {
        qCInfo(HubLog) << "Cannot select invalid device:" << index;
        return;
    }

    mSelectedDeviceIndex = index;

    qCInfo(HubLog) << "Selected device:" << mSelectedDeviceIndex;
}

void Hub::processStatusResponse(const protocol_msg_t& msg)
{
    qCInfo(HubLog) << "Status:";
    qCInfo(HubLog) << "  Firmware version:" << msg.payload.status.firmware_ver;

    if (mReadStatusCallback)
    {
        RemoteDeviceStatus status;
        status.firmwareVersion = std::string{ msg.payload.status.firmware_ver };

        status.lastCommands.reserve(10);
        for (auto it = std::begin(msg.payload.status.last_commands); it != std::end(msg.payload.status.last_commands); ++it)
        {
            status.lastCommands.push_back(*it);
        }

        mReadStatusCallback(std::move(status));
        mReadStatusCallback = {};
    }

    mState = State::Idle;
}

void BlynkOnDisconnected()
{
    qCInfo(HubLog) << "Blynk disconnected";
}

void BlynkOnConnected()
{
    qCInfo(HubLog) << "Blynk connected";
}

template<class Proto>
BLYNK_FORCE_INLINE
void BlynkApi<Proto>::sendInfo()
{
    static const char profile[] BLYNK_PROGMEM = "blnkinf\0"
#ifdef BOARD_FIRMWARE_VERSION
        BLYNK_PARAM_KV("ver"    , BOARD_FIRMWARE_VERSION)
        BLYNK_PARAM_KV("blynk"  , BLYNK_VERSION)
#else
        BLYNK_PARAM_KV("ver"    , BLYNK_VERSION)
#endif
#ifdef BOARD_TEMPLATE_ID
        BLYNK_PARAM_KV("tmpl"   , BOARD_TEMPLATE_ID)
#endif
        BLYNK_PARAM_KV("h-beat" , BLYNK_TOSTRING(BLYNK_HEARTBEAT))
        BLYNK_PARAM_KV("buff-in", BLYNK_TOSTRING(BLYNK_MAX_READBYTES))
#ifdef BLYNK_INFO_DEVICE
        BLYNK_PARAM_KV("dev"    , BLYNK_INFO_DEVICE)
#endif
#ifdef BLYNK_INFO_CPU
        BLYNK_PARAM_KV("cpu"    , BLYNK_INFO_CPU)
#endif
#ifdef BLYNK_INFO_CONNECTION
        BLYNK_PARAM_KV("con"    , BLYNK_INFO_CONNECTION)
#endif
        BLYNK_PARAM_KV("build"  , __DATE__ " " __TIME__)
        "\0"
    ;
    const size_t profile_len = sizeof(profile)-8-2;

    char mem_dyn[32];
    BlynkParam profile_dyn(mem_dyn, 0, sizeof(mem_dyn));
    profile_dyn.add_key("conn", "Socket");

    static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_INTERNAL, 0, profile+8, profile_len, profile_dyn.getBuffer(), profile_dyn.getLength());
    return;
}

template<class Proto>
BLYNK_FORCE_INLINE
void BlynkApi<Proto>::processCmd(const void* buff, size_t len)
{
    BlynkParam param((void*)buff, len);
    BlynkParam::iterator it = param.begin();
    if (it >= param.end())
        return;
    const char* cmd = it.asStr();
    uint16_t cmd16;
    memcpy(&cmd16, cmd, sizeof(cmd16));
    if (++it >= param.end())
        return;

    const uint8_t pin = it.asInt();

    switch(cmd16) {

#ifndef BLYNK_NO_BUILTIN

    case BLYNK_HW_PM: {
        while (it < param.end()) {
            ++it;
#ifdef BLYNK_DEBUG
            BLYNK_LOG4(BLYNK_F("Invalid pin "), pin, BLYNK_F(" mode "), it.asStr());
#endif
            ++it;
        }
    } break;
    case BLYNK_HW_DR: {
        char mem[16];
        BlynkParam rsp(mem, 0, sizeof(mem));
        rsp.add("dw");
        rsp.add(pin);
        rsp.add(0); // TODO
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_HARDWARE, 0, rsp.getBuffer(), rsp.getLength()-1);
    } break;
    case BLYNK_HW_DW: {
        // Should be 1 parameter (value)
        if (++it >= param.end())
            return;

        // TODO: digitalWrite(pin, it.asInt() ? HIGH : LOW);
    } break;
    case BLYNK_HW_AW: {
        // Should be 1 parameter (value)
        if (++it >= param.end())
            return;

        // TODO: analogWrite(pin, it.asInt());
    } break;

#endif

    case BLYNK_HW_VR: {
        BlynkReq req = { pin };
        WidgetReadHandler handler = GetReadHandler(pin);
        if (handler && (handler != BlynkWidgetRead)) {
            handler(req);
        } else {
            BlynkWidgetReadDefault(req);
        }
    } break;
    case BLYNK_HW_VW: {
        ++it;
        char* start = (char*)it.asStr();
        BlynkParam param2(start, len - (start - (char*)buff));
        BlynkReq req = { pin };
        WidgetWriteHandler handler = GetWriteHandler(pin);
        if (handler && (handler != BlynkWidgetWrite)) {
            handler(req, param2);
        } else {
            BlynkWidgetWriteDefault(req, param2);
        }
    } break;
    default:
        BLYNK_LOG2(BLYNK_F("Invalid HW cmd: "), cmd);
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_RESPONSE, static_cast<Proto*>(this)->msgIdOutOverride, NULL, BLYNK_ILLEGAL_COMMAND);
    }
}

// FIXME

BLYNK_INPUT(V73) {
    s_hub->selectDevice(0);
    s_hub->shutter1Up();
}

BLYNK_INPUT(V72) {
    s_hub->selectDevice(0);
    s_hub->shutter1Down();
}

BLYNK_INPUT(V71) {
    s_hub->selectDevice(1);
    s_hub->shutter2Up();
}

BLYNK_INPUT(V70) {
    s_hub->selectDevice(1);
    s_hub->shutter2Down();
}

BLYNK_INPUT(V69) {
    s_hub->selectDevice(1);
    s_hub->shutter1Up();
}

BLYNK_INPUT(V68) {
    s_hub->selectDevice(1);
    s_hub->shutter1Down();
}

BLYNK_INPUT(V67) {
    s_hub->selectDevice(2);
    s_hub->shutter1Up();
}

BLYNK_INPUT(V66) {
    s_hub->selectDevice(2);
    s_hub->shutter1Down();
}
