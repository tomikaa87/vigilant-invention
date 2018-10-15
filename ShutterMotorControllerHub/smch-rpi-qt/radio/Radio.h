/*
 * Radio.h
 *
 *  Created on: 2018. maj. 26.
 *      Author: tomikaa
 */

#pragma once

#include <QObject>

#include "nrf24.h"
#include "radio_protocol.h"
#include "IRadio.h"

namespace radio
{

class Radio: public QObject, public IRadio
{
    Q_OBJECT

public:
    explicit Radio(QObject* parent = nullptr);

    // IRadio interface
public:
    std::future<Response> sendCommand(Command command, const std::string& address) override;
    std::future<Response> readStatus(const std::string& address) override;

private:
    nrf24_t m_nrf;

    enum class InterruptResult
    {
        NoInterrupt,
        DataSent,
        DataReceived,
        PacketLost
    };

    InterruptResult checkInterrupt();
    bool isInterruptTriggered() const;
};

//class Radio: public QObject
//{
//    Q_OBJECT

//public:
//    Radio(QObject* parent = nullptr);

//    enum class TaskResult
//    {
//        None,
//        Busy,
//        PacketLost,
//        DataReceived,
//        DataSent
//    };

//    TaskResult task();

//    void sendMessage(
//            const uint8_t* address,
//            const protocol_msg_t* msg,
//            const uint8_t addressLen = 5);

//    struct Stats
//    {
//        uint32_t sentPackets = 0;
//        uint32_t receivedPackets = 0;
//        uint32_t lostPackets = 0;
//    };

//    const protocol_msg_t& lastReceivedMessage() const;

//private:
//    nrf24_t* mNrf24 = nullptr;
//    uint8_t mChannel = 0;
//    Stats mStats = {};
//    bool mBusy = false;
//    protocol_msg_t mLastReceivedMessage;

//    void initTransceiver(uint8_t channel);

//    enum class RadioMode
//    {
//        PrimaryTransmitter,
//        PrimaryReceiver
//    };

//    void setRadioMode(
//            const RadioMode mode,
//            const uint8_t* txAddress = nullptr,
//            const uint8_t txAddressLen = 5);

//    void packetLost();
//    void dataReceived();
//    void dataSent();

//    void processIncomingMessage(protocol_msg_t* msg);

//    void printStats();

//    void resetPacketLossCounter();
//};

}
