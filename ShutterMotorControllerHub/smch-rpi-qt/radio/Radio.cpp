/*
 * Radio.cpp
 *
 *  Created on: 2018. maj. 26.
 *      Author: tomikaa
 */

#include "Radio.h"
#include "Utils.h"
#include "Task.h"

#ifndef WIN32
#include "wiringPi.h"
#include "wiringPiSPI.h"
#endif

#include <QLoggingCategory>
Q_LOGGING_CATEGORY(RadioLog, "Radio")

#include <chrono>
#include <cstdio>

namespace radio
{

#define ENABLE_DEBUG_LOG

static const uint8_t ReceiveAddress[] = { 'S', 'M', 'R', 'H', '1' };

static const int NRF_CE = 4;
static const int NRF_CSN = 6;
static const int NRF_IRQ = 5;

Radio::Radio(uint8_t transceiverChannel, QObject* parent)
    : QObject{ parent }
    , m_transceiverChannel{ transceiverChannel }
{
    initTransceiver();
}

std::future<Response> Radio::sendCommand(Command command, const std::string& address)
{
    auto task = std::make_shared<Task>(createCommandRequest(command, address));

    m_queue.enqueue([this, task] {
        auto response = sendRequest(task->request);
        task->promise.set_value(response);
    });

    return task->promise.get_future();
}

std::future<Response> Radio::readStatus(const std::string& address)
{
    auto task = std::make_shared<Task>(createStatusRequest(address));

    m_queue.enqueue([this, task] {
        auto response = sendRequest(task->request);
        task->promise.set_value(response);
    });

    return task->promise.get_future();
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

    nrf24_clear_interrupts(&m_nrf);

#if defined ENABLE_DEBUG_LOG && defined NRF24_ENABLE_DUMP_REGISTERS
    nrf24_dump_registers(&m_nrf);
#endif
}

void Radio::setTransceiverMode(const TransceiverMode mode, const std::string& txAddress)
{
    qCDebug(RadioLog) << "setting mode to" << (mode == TransceiverMode::PrimaryReceiver ? "PRX" : "PTX");

    if (!txAddress.empty())
        qCDebug(RadioLog).nospace() << "transmit address: " << txAddress.c_str() << " (length: " << txAddress.size() << ")";

    nrf24_power_down(&m_nrf);

    nrf24_clear_interrupts(&m_nrf);

    // Reset packet loss counter PLOS_CNT
    nrf24_set_rf_channel(&m_nrf, m_transceiverChannel);

    nrf24_flush_rx(&m_nrf);
    nrf24_flush_tx(&m_nrf);

    nrf24_config_t config;
    memset(&config, 0, sizeof (nrf24_config_t));
    config.EN_CRC = 1;
    config.PWR_UP = 1;

    if (mode == TransceiverMode::PrimaryReceiver)
    {
        nrf24_set_rx_address(&m_nrf, 0, ReceiveAddress, sizeof(ReceiveAddress));
        config.PRIM_RX = 1;
    }
    else
    {
        nrf24_set_tx_address(&m_nrf, reinterpret_cast<const uint8_t*>(txAddress.c_str()), txAddress.size() & 0x07);
        nrf24_set_rx_address(&m_nrf, 0, reinterpret_cast<const uint8_t*>(txAddress.c_str()), txAddress.size() & 0x07);
    }

    nrf24_set_config(&m_nrf, config);

    nrf24_dump_registers(&m_nrf);
}

std::list<protocol_msg_t> Radio::readIncomingMessages()
{
    qCDebug(RadioLog) << "data received";

    std::list<protocol_msg_t> messages;

    while (1)
    {
        auto fifoStatus = nrf24_get_fifo_status(&m_nrf);

        if (fifoStatus.RX_EMPTY)
        {
            qCDebug(RadioLog) << "RX FIFO empty";
            break;
        }

        qCDebug(RadioLog) << "reading incoming payload";

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

Radio::InterruptResult Radio::checkInterrupt()
{
    if (isInterruptTriggered())
    {
        auto status = nrf24_get_status(&m_nrf);

        nrf24_dump_registers(&m_nrf);

        nrf24_clear_interrupts(&m_nrf);

        if (status.MAX_RT)
            return InterruptResult::PacketLost;

        if (status.RX_DR)
            return InterruptResult::DataReceived;

        if (status.TX_DS)
            return InterruptResult::DataSent;
    }

    return InterruptResult::Timeout;
}

bool Radio::isInterruptTriggered() const
{
#ifdef WIN32
    return false;
#else
    return digitalRead(NRF_IRQ) == 0;
#endif
}

Radio::InterruptResult Radio::waitForInterrupt()
{
    qCDebug(RadioLog) << "waiting for interrupt";

    auto startTime = std::chrono::steady_clock::now();
    auto result = InterruptResult::Timeout;

    while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(2))
    {
        if (isInterruptTriggered())
        {
            result = checkInterrupt();
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    qCDebug(RadioLog) << "interrupt result:" << result;

    return result;
}

Result Radio::sendProtocolMsg(const std::string& address, const protocol_msg_t& msg)
{
    qCDebug(RadioLog) << "sending protocol message to" << address.c_str();

    setTransceiverMode(TransceiverMode::PrimaryTransmitter, address);

    nrf24_transmit_data(&m_nrf, msg.data, sizeof(protocol_msg_t));
    nrf24_power_up(&m_nrf);

    auto interruptResult = waitForInterrupt();

    if (interruptResult == InterruptResult::Timeout)
        return Result::Timeout;

    if (interruptResult == InterruptResult::PacketLost)
        return Result::PacketLost;

    assert(interruptResult == InterruptResult::DataSent);

    return Result::Succeeded;
}

Result Radio::receiveProtocolMessages(std::list<protocol_msg_t>& messages)
{
    qCDebug(RadioLog) << "receiving protocol messages";

    setTransceiverMode(TransceiverMode::PrimaryReceiver);

    nrf24_power_up(&m_nrf);

    auto interruptResult = waitForInterrupt();

    if (interruptResult == InterruptResult::Timeout)
        return Result::Timeout;

    assert(interruptResult == InterruptResult::DataReceived);

    messages = readIncomingMessages();

    qCDebug(RadioLog) << "number of received messages:" << messages.size();

    return Result::Succeeded;
}

Response Radio::sendRequest(const Request& request)
{
    Response response;

    response.result = sendProtocolMsg(request.address, request.msg);

    if (response.result != Result::Succeeded)
        return response;

    response.result = receiveProtocolMessages(response.messages);

    return response;
}

QDebug& operator<<(QDebug& dbg, Radio::InterruptResult ir)
{
    switch (ir)
    {
    case Radio::InterruptResult::DataReceived:
        dbg << "DataReceived";
        break;

    case Radio::InterruptResult::DataSent:
        dbg << "DataSent";
        break;

    case Radio::InterruptResult::Timeout:
        dbg << "Timeout";
        break;

    case Radio::InterruptResult::PacketLost:
        dbg << "PacketLost";
        break;
    }

    return dbg;
}

}
