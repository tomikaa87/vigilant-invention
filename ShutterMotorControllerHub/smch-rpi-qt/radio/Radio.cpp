/*
 * Radio.cpp
 *
 *  Created on: 2018. maj. 26.
 *      Author: tomikaa
 */

#include "Radio.h"
#include "nrf24.h"
#include "Utils.h"
#include "Task.h"

#ifndef WIN32
#include "wiringPi.h"
#include "wiringPiSPI.h"
#endif

#include <QLoggingCategory>
Q_LOGGING_CATEGORY(RadioLog, "Radio")

#include <cstdio>

namespace radio
{

#define ENABLE_DEBUG_LOG

static const uint8_t ReceiveAddress[] = { 'S', 'M', 'R', 'H', '1' };

static const int NRF_CE = 4;
static const int NRF_CSN = 6;
static const int NRF_IRQ = 5;

Radio::InterruptResult Radio::checkInterrupt()
{
    if (isInterruptTriggered())
    {
        auto status = nrf24_get_status(&m_nrf);

        nrf24_clear_interrupts(&m_nrf);

        if (status.MAX_RT)
            return InterruptResult::PacketLost;

        if (status.RX_DR)
            return InterruptResult::DataReceived;

        if (status.TX_DS)
            return InterruptResult::DataSent;
    }

    return InterruptResult::NoInterrupt;
}

bool Radio::isInterruptTriggered() const
{
#ifdef WIN32
    return false;
#else
    return digitalRead(NRF_IRQ) == 0;
#endif
}

//void Radio::sendMessage(
//        const uint8_t* address,
//        const protocol_msg_t* msg,
//        const uint8_t addressLen)
//{
//    mBusy = true;

//#ifdef ENABLE_DEBUG_LOG
//    qCDebug(RadioLog) << "sending message:";
//    print_protocol_message(msg);
//#endif

//    setRadioMode(RadioMode::PrimaryTransmitter, address, addressLen);
//    nrf24_transmit_data(&m_nrf, msg->data, sizeof(protocol_msg_t));
//    nrf24_power_up(&m_nrf);
//}

Radio::Radio(uint8_t transceiverChannel, QObject* parent)
    : QObject{ parent }
    , m_transceiverChannel{ transceiverChannel }
{
    initTransceiver();
}

std::future<Response> Radio::sendCommand(Command command, const std::string& address)
{
    auto task = std::make_shared<Task>(createCommandRequest(command, address));

    m_queue.enqueue([this, task{ std::move(task) }] {
        Response response;

        setTransceiverMode(RadioMode::PrimaryTransmitter, task->request.address);
        nrf24_transmit_data(&m_nrf, task->request.msg.data, sizeof(protocol_msg_t));
        nrf24_power_up(&m_nrf);

        while (!isInterruptTriggered() /*&& !timeout*/)
            continue;

        if (!isInterruptTriggered())
        {
            response.result = Result::PacketLost;
        }
        else
        {
            response.result = Result::Succeeded;
            response.messages = readIncomingMessages();
        }

        task->promise.set_value(response);
    });

    return task->promise.get_future();
}

std::future<Response> Radio::readStatus(const std::string& address)
{

}

void Radio::initTransceiver()
{
    qCDebug(RadioLog) << "initializing transceiver, channel:" << m_transceiverChannel;

#ifndef WIN32
    pinMode(NRF_CE, OUTPUT);
    pinMode(NRF_CSN, OUTPUT);
    pinMode(NRF_IRQ, INPUT);
#endif

    nrf24_init(&m_nrf,
        [](nrf24_state_t state) {
#ifndef WIN32
            digitalWrite(NRF_CE, state == NRF24_HIGH ? HIGH : LOW);
#endif
        },
        [](nrf24_state_t state) {
#ifndef WIN32
            digitalWrite(NRF_CSN, state == NRF24_HIGH ? HIGH : LOW);
#endif
        },
        [](uint8_t data) -> uint8_t {
#ifndef WIN32
            wiringPiSPIDataRW(0, &data, 1);
            return data;
#else
            return 0;
#endif
        });

    nrf24_rf_setup_t rf_setup;
    memset(&rf_setup, 0, sizeof (nrf24_rf_setup_t));
    rf_setup.P_RF_DR_HIGH = 0;
    rf_setup.P_RF_DR_LOW = 1;
    rf_setup.P_RF_PWR = NRF24_RF_OUT_PWR_0DBM;
    nrf24_set_rf_setup(&m_nrf, rf_setup);

    nrf24_set_rf_channel(&m_nrf, m_transceiverChannel);

    nrf24_setup_retr_t setup_retr;
    setup_retr.ARC = 15;
    setup_retr.ARD = 15;
    nrf24_set_setup_retr(&m_nrf, setup_retr);

    nrf24_set_rx_payload_len(&m_nrf, 0, NRF24_DEFAULT_PAYLOAD_LEN);

#if defined ENABLE_DEBUG_LOG && defined NRF24_NRF24_ENABLE_DUMP_REGISTERS
    nrf24_dump_registers(&m_nrf);
#endif
}

void Radio::setTransceiverMode(const RadioMode mode, const std::string& txAddress)
{
    qCDebug(RadioLog) << "setting mode to" << (mode == RadioMode::PrimaryReceiver ? "PRX" : "PTX");

    nrf24_power_down(&m_nrf);

    resetTransceiverPacketLossCounter();

    nrf24_config_t config;
    memset(&config, 0, sizeof (nrf24_config_t));
    config.EN_CRC = 1;
    config.PWR_UP = 1;

    if (mode == RadioMode::PrimaryReceiver)
    {
        nrf24_set_rx_address(&m_nrf, 0, ReceiveAddress, sizeof(ReceiveAddress));
        config.PRIM_RX = 1;
    }
    else
    {
        resetTransceiverPacketLossCounter();
        nrf24_set_tx_address(&m_nrf, reinterpret_cast<const uint8_t*>(txAddress.c_str()), txAddress.size() & 0x07);
        nrf24_set_rx_address(&m_nrf, 0, reinterpret_cast<const uint8_t*>(txAddress.c_str()), txAddress.size() & 0x07);
    }

    nrf24_set_config(&m_nrf, config);
    nrf24_power_up(&m_nrf);
}

//void Radio::packetLost()
//{
////    DEBUG(Radio, "packet lost");
//    qCWarning(RadioLog) << "packet lost";
//    ++mStats.lostPackets;

//    auto otx = nrf24_get_observe_tx(&m_nrf);
//    qCDebug(RadioLog) << "current packet loss count:" << otx.PLOS_CNT;

//    if (otx.PLOS_CNT == 15)
//        qCWarning(RadioLog) << "max packet loss count reached";

//    setRadioMode(RadioMode::PrimaryReceiver);
//}

std::list<protocol_msg_t> Radio::readIncomingMessages()
{
    qCDebug(RadioLog) << "data received";
//    ++mStats.receivedPackets;

    std::list<protocol_msg_t> messages;

    while (1)
    {
        qCDebug(RadioLog) << "reading incoming payload";

        auto fifoStatus = nrf24_get_fifo_status(&m_nrf);

        if (fifoStatus.RX_EMPTY)
        {
            qCDebug(RadioLog) << "RX FIFO empty";
            break;
        }

        protocol_msg_t msg;

        nrf24_read_rx_payload(&m_nrf, msg.data, sizeof(protocol_msg_t));

        if (msg.msg_magic != PROTO_MSG_MAGIC)
        {
            qCWarning(RadioLog) << "incoming protocol message is invalid";
            break;
        }

        messages.push_back(std::move(msg));
    };

    return messages;
}

//void Radio::dataSent()
//{
//    qCDebug(RadioLog) << "data sent";
//    ++mStats.sentPackets;

//    setRadioMode(RadioMode::PrimaryReceiver);
//}

//void Radio::processIncomingMessage(protocol_msg_t* msg)
//{
//    // TODO maybe more than one message sould be stored
//    mLastReceivedMessage = *msg;

//#ifdef ENABLE_DEBUG_LOG
//    print_protocol_message(msg);
//#endif
//}

//void Radio::printStats()
//{
//    qCDebug(RadioLog, "sent: %u, recvd: %u, lost: %u",
//            mStats.sentPackets,
//            mStats.receivedPackets,
//            mStats.lostPackets);
//}

void Radio::resetTransceiverPacketLossCounter()
{
    qCDebug(RadioLog) << "resetting packet loss counter";

    // According to the documentation, setting RF_CH resets PLOS_CNT
    nrf24_set_rf_channel(&m_nrf, m_transceiverChannel);
}

//const protocol_msg_t& Radio::lastReceivedMessage() const
//{
//    return mLastReceivedMessage;
//}

}
