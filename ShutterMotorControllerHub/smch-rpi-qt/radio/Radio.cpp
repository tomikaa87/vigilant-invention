/*
 * Radio.cpp
 *
 *  Created on: 2018. maj. 26.
 *      Author: tomikaa
 */

#include "Radio.h"
#include "nrf24.h"

//#include "wiringPi.h"
//#include "wiringPiSPI.h"

#include <QLoggingCategory>
Q_LOGGING_CATEGORY(RadioLog, "Radio")

#include <cstdio>

namespace radio
{

//#define ENABLE_DEBUG_LOG

//static const uint8_t ReceiveAddress[] = { 'S', 'M', 'R', 'H', '1' };

//static const int NRF_CE = 4;
//static const int NRF_CSN = 6;
//static const int NRF_IRQ = 5;

//Radio::Radio(QObject *parent)
//    : QObject(parent)
//    , mNrf24(new nrf24_t)
//{
//    initTransceiver(0);
//}

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
//    nrf24_transmit_data(mNrf24, msg->data, sizeof(protocol_msg_t));
//    nrf24_power_up(mNrf24);
//}

//void Radio::initTransceiver(uint8_t channel)
//{
//    qCDebug(RadioLog) << "initializing transceiver, channel:" << channel;

//    pinMode(NRF_CE, OUTPUT);
//    pinMode(NRF_CSN, OUTPUT);
//    pinMode(NRF_IRQ, INPUT);

//    nrf24_init(mNrf24,
//        [](nrf24_state_t state) {
//            digitalWrite(NRF_CE, state == NRF24_HIGH ? HIGH : LOW);
//        },
//        [](nrf24_state_t state) {
//            digitalWrite(NRF_CSN, state == NRF24_HIGH ? HIGH : LOW);
//        },
//        [](uint8_t data) -> uint8_t {
//            wiringPiSPIDataRW(0, &data, 1);
//            return data;
//        });

//    nrf24_rf_setup_t rf_setup;
//    memset(&rf_setup, 0, sizeof (nrf24_rf_setup_t));
//    rf_setup.P_RF_DR_HIGH = 0;
//    rf_setup.P_RF_DR_LOW = 1;
//    rf_setup.P_RF_PWR = NRF24_RF_OUT_PWR_0DBM;
//    nrf24_set_rf_setup(mNrf24, rf_setup);

//    nrf24_set_rf_channel(mNrf24, channel);
//    mChannel = channel;

//    nrf24_setup_retr_t setup_retr;
//    setup_retr.ARC = 15;
//    setup_retr.ARD = 15;
//    nrf24_set_setup_retr(mNrf24, setup_retr);

//    nrf24_set_rx_payload_len(mNrf24, 0, NRF24_DEFAULT_PAYLOAD_LEN);

//#if defined ENABLE_DEBUG_LOG && defined NRF24_NRF24_ENABLE_DUMP_REGISTERS
//    nrf24_dump_registers(mNrf24);
//#endif
//}

//Radio::TaskResult Radio::task()
//{
//    auto result = TaskResult::None;

//    if (digitalRead(NRF_IRQ) == 0)
//    {
//        qCDebug(RadioLog) << "NRF interrupt";

//        auto status = nrf24_get_status(mNrf24);

//        nrf24_clear_interrupts(mNrf24);

//        if (status.MAX_RT)
//        {
//            packetLost();
//            result = TaskResult::PacketLost;
//            mBusy = false;
//        }

//        if (status.RX_DR)
//        {
//            dataReceived();
//            result = TaskResult::DataReceived;
//            mBusy = false;
//        }

//        if (status.TX_DS)
//        {
//            dataSent();
//            result = TaskResult::DataSent;
//            mBusy = false;
//        }

//        printStats();
//    }

//    if (mBusy)
//        result = TaskResult::Busy;

//    return result;
//}

//void Radio::setRadioMode(
//        const RadioMode mode,
//        const uint8_t* txAddress,
//        const uint8_t txAddressLen)
//{
//    qCDebug(RadioLog) << "setting mode to" << (mode == RadioMode::PrimaryReceiver ? "PRX" : "PTX");

//    nrf24_power_down(mNrf24);

//    resetPacketLossCounter();

//    nrf24_config_t config;
//    memset(&config, 0, sizeof (nrf24_config_t));
//    config.EN_CRC = 1;
//    config.PWR_UP = 1;

//    if (mode == RadioMode::PrimaryReceiver)
//    {
//        nrf24_set_rx_address(mNrf24, 0, ReceiveAddress, sizeof(ReceiveAddress));
//        config.PRIM_RX = 1;
//    }
//    else if (txAddress)
//    {
//        resetPacketLossCounter();
//        nrf24_set_tx_address(mNrf24, txAddress, txAddressLen);
//        nrf24_set_rx_address(mNrf24, 0, txAddress, txAddressLen);
//    }

//    nrf24_set_config(mNrf24, config);
//    nrf24_power_up(mNrf24);
//}

//void Radio::packetLost()
//{
////    DEBUG(Radio, "packet lost");
//    qCWarning(RadioLog) << "packet lost";
//    ++mStats.lostPackets;

//    auto otx = nrf24_get_observe_tx(mNrf24);
//    qCDebug(RadioLog) << "current packet loss count:" << otx.PLOS_CNT;

//    if (otx.PLOS_CNT == 15)
//        qCWarning(RadioLog) << "max packet loss count reached";

//    setRadioMode(RadioMode::PrimaryReceiver);
//}

//void Radio::dataReceived()
//{
//    qCDebug(RadioLog) << "data received";
//    ++mStats.receivedPackets;

//    while (1)
//    {
//        qCDebug(RadioLog) << "reading incoming payload";

//        auto fifo_status = nrf24_get_fifo_status(mNrf24);

//        if (fifo_status.RX_EMPTY)
//        {
//            qCDebug(RadioLog) << "RX FIFO empty";
//            break;
//        }

//        protocol_msg_t msg;

//        nrf24_read_rx_payload(mNrf24, msg.data, sizeof(protocol_msg_t));

//        if (msg.msg_magic != PROTO_MSG_MAGIC)
//        {
//            qCWarning(RadioLog) << "incoming protocol message is invalid";
//            return;
//        }

//        // TODO: add incoming message to a queue instead of processing immediately
//        processIncomingMessage(&msg);
//    };
//}

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

//void Radio::resetPacketLossCounter()
//{
//    qCDebug(RadioLog) << "resetting packet loss counter";

//    // According to the documentation, setting RF_CH resets PLOS_CNT
//    nrf24_set_rf_channel(mNrf24, mChannel);
//}

//const protocol_msg_t& Radio::lastReceivedMessage() const
//{
//    return mLastReceivedMessage;
//}

}
