/*
 * Radio.h
 *
 *  Created on: 2018. máj. 26.
 *      Author: tomikaa
 */

#ifndef HUB_RADIO_H_
#define HUB_RADIO_H_

#include <hub/nrf24.h>
#include <hub/radio_protocol.h>

class Radio
{
public:
    Radio();

    enum class TaskResult
    {
        None,
        Busy,
        PacketLost,
        DataReceived,
        DataSent
    };

    TaskResult task();

    void sendMessage(
            const uint8_t* address,
            const protocol_msg_t* msg,
            const uint8_t addressLen = 5);

    struct Stats
    {
        uint32_t sentPackets;
        uint32_t receivedPackets;
        uint32_t lostPackets;
    };

    const protocol_msg_t& lastReceivedMessage() const;

private:
    nrf24_t* mNrf24 = nullptr;
    Stats mStats = { 0 };
    bool mBusy = false;
    protocol_msg_t mLastReceivedMessage;

    void initTransceiver(uint8_t channel);

    enum class RadioMode
    {
        PrimaryTransmitter,
        PrimaryReceiver
    };

    void setRadioMode(
            const RadioMode mode,
            const uint8_t* txAddress = nullptr,
            const uint8_t txAddressLen = 5);

    void packetLost();
    void dataReceived();
    void dataSent();

    void processIncomingMessage(protocol_msg_t* msg);

    void printStats();
};

#endif /* HUB_RADIO_H_ */
