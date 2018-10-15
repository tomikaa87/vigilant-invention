/*
 * Radio.h
 *
 *  Created on: 2018. maj. 26.
 *      Author: tomikaa
 */

#pragma once

#include <QDebug>
#include <QObject>

#include "IRadio.h"
#include "nrf24.h"
#include "radio_protocol.h"
#include "Response.h"
#include "Request.h"
#include "OperationQueue.h"

namespace radio
{

class Radio: public QObject, public IRadio
{
    Q_OBJECT

public:
    Radio(uint8_t transceiverChannel, QObject* parent = nullptr);

    // IRadio interface
public:
    std::future<Response> sendCommand(Command command, const std::string& address) override;
    std::future<Response> readStatus(const std::string& address) override;

private:
    enum class InterruptResult
    {
        Timeout,
        DataSent,
        DataReceived,
        PacketLost
    };

    enum class TransceiverMode
    {
        PrimaryTransmitter,
        PrimaryReceiver
    };

    nrf24_t m_nrf;
    uint8_t m_transceiverChannel = 0;
    OperationQueue m_queue;

    void initTransceiver();
    void setTransceiverMode(const TransceiverMode mode, const std::string& txAddress = {});
    std::list<protocol_msg_t> readIncomingMessages();

    InterruptResult checkInterrupt();
    bool isInterruptTriggered() const;
    InterruptResult waitForInterrupt();
    Result sendProtocolMsg(const std::string& address, const protocol_msg_t& msg);
    Result receiveProtocolMessages(std::list<protocol_msg_t>& messages);
    Response sendRequest(const Request& request);

    friend QDebug& operator<<(QDebug& dbg, Radio::InterruptResult ir);
};

}
