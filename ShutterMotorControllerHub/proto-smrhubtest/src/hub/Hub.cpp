/*
 * Hub.cpp
 *
 *  Created on: 2018. máj. 26.
 *      Author: tomikaa
 */

#include <hub/Hub.h>
#include <hub/Logging.h>

Hub::Hub()
{
}

void Hub::task()
{
    auto result = mRadio.task();

    switch (result)
    {
        case Radio::TaskResult::Busy:
            DEBUG(Hub, "Radio is busy");
            break;

        case Radio::TaskResult::PacketLost:
            DEBUG(Hub, "Packet lost");
            break;

        case Radio::TaskResult::DataSent:
            DEBUG(Hub, "Data sent");
            break;

        case Radio::TaskResult::DataReceived:
            DEBUG(Hub, "Data received");
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
        INFO(Hub, "Cannot send message to device %u while Hub is busy",
                mSelectedDeviceIndex);
        return;
    }

    INFO(Hub, "Sending message to device %u", mSelectedDeviceIndex);

    char address[] = "SMRR0";
    address[4] = '0' + mSelectedDeviceIndex;

    mRadio.sendMessage((const uint8_t*)address, msg);

    mState = State::SendingMessage;
    mTimer.reset();
}

void Hub::readStatus()
{
    protocol_msg_t msg;
    protocol_msg_init(&msg);
    msg.msg_type = PROTO_MSG_READ_STATUS;

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

void Hub::scanUnits()
{
    if (mState != State::Idle)
    {
        INFO(Hub, "Cannot initiate scan while Hub is busy");
        return;
    }

    INFO(Hub, "Scanning SMRR units");

    mState = State::Scanning;
    mScanState = ScanState::Starting;
}

void Hub::progressScanning(Radio::TaskResult radioTaskResult)
{
    switch (mScanState)
    {
        case ScanState::Idle:
            break;

        case ScanState::Starting:
        {
            DEBUG(Hub, "Starting scan");
            mScanDeviceIndex = 0;
            mScanState = ScanState::SendStatusRequest;
            break;
        }

        case ScanState::SendStatusRequest:
        {
            if (mScanRetryCount >= 3)
            {
                DEBUG(Hub, "Max retry count reached");
                mActiveDevices[mScanDeviceIndex] = false;
                ++mScanDeviceIndex;
                mScanRetryCount = 0;
            }

            if (mScanDeviceIndex >= 10)
            {
                DEBUG(Hub, "Max device index reached");
                mScanState = ScanState::Idle;
                mState = State::Idle;

                INFO(Hub, "Scan finished");
                INFO(Hub, "Active devices:");
                for (uint8_t i = 0; i < sizeof(mActiveDevices); ++i)
                {
                    if (!mActiveDevices[i])
                        continue;

                    INFO(Hub, "  SMRR%c", '0' + i);
                }

                break;
            }

            protocol_msg_t msg;
            protocol_msg_init(&msg);
            msg.msg_type = PROTO_MSG_READ_STATUS;

            char address[] = "SMRR0";
            address[4] = '0' + mScanDeviceIndex;

            DEBUG(Hub, "Requesting status from %s", address);

            mRadio.sendMessage((const uint8_t*)address, &msg);

            mScanState = ScanState::WaitingForStatusPacket;
            mTimer.reset();
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
                DEBUG(Hub, "Response timeout");
                mScanState = ScanState::StatusRequestPacketLost;
            }

            break;
        }

        case ScanState::StatusRequestPacketLost:
        {
            DEBUG(Hub, "Status request packet lost");
            ++mScanRetryCount;
            mScanState = ScanState::SendStatusRequest;
            break;
        }

        case ScanState::ProcessStatusPacket:
        {
            DEBUG(Hub, "Device found");
            mActiveDevices[mScanDeviceIndex] = true;
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
            // TODO since this is a generic response handler,
            // status message responses should be processed separately

            auto&& msg = mRadio.lastReceivedMessage();

            INFO(Hub, "Status:");
            INFO(Hub, "  Firmware version: %s", msg.payload.status.firmware_ver);

            mState = State::Idle;

            break;
        }

        case Radio::TaskResult::PacketLost:
        {
            INFO(Hub, "Status request packet lost");
            mState = State::Idle;
        }
        [[fallthrough]]

        default:
        {
            if (mTimer.elapsed() > 200)
            {
                INFO(Hub, "Status request timed out");
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
        INFO(Hub, "Cannot select invalid device: %u", index);
        return;
    }

    mSelectedDeviceIndex = index;

    INFO(Hub, "Selected device: %u", mSelectedDeviceIndex);
}
