/*
 * Radio.cpp
 *
 *  Created on: 2018. máj. 26.
 *      Author: tomikaa
 */

#include <hub/Radio.h>
#include <hub/nrf24.h>
#include <stm32f4xx.h>
#include <hub/Logging.h>

#include <stdio.h>

static const uint8_t ReceiveAddress[] = { 'S', 'M', 'R', 'H', '1' };

Radio::Radio()
    : mNrf24(new nrf24_t)
{
    initTransceiver(0);
}

void Radio::sendMessage(
        const uint8_t* address,
        const protocol_msg_t* msg,
        const uint8_t addressLen)
{
    mBusy = true;

#ifdef ENABLE_DEBUG_LOG
    DEBUG(Radio, "sending message:");
    print_protocol_message(msg);
#endif

    setRadioMode(RadioMode::PrimaryTransmitter, address, addressLen);
    nrf24_transmit_data(mNrf24, msg->data, sizeof(protocol_msg_t));
    nrf24_power_up(mNrf24);
}

void Radio::initTransceiver(uint8_t channel)
{
    DEBUG(Radio, "initializing transceiver, channel: %d", channel);

    static const int NRF_CE = GPIO_Pin_8;
    static const int NRF_CSN = GPIO_Pin_7;

    nrf24_init(mNrf24,
        [](nrf24_state_t state) {
            if (state == NRF24_HIGH)
                GPIO_SetBits(GPIOB, NRF_CE);
            else
                GPIO_ResetBits(GPIOB, NRF_CE);

            for (int i = 0; i < 5000; ++i)
                asm("NOP");
        },
        [](nrf24_state_t state) {
            if (state == NRF24_HIGH)
                GPIO_SetBits(GPIOB, NRF_CSN);
            else
                GPIO_ResetBits(GPIOB, NRF_CSN);

            for (int i = 0; i < 5000; ++i)
                asm("NOP");
        },
        [](uint8_t data) -> uint8_t {
            while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
            SPI_I2S_SendData(SPI2, data);

            while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE));
            uint8_t input = SPI_I2S_ReceiveData(SPI2);

            return input;
        });

    nrf24_rf_setup_t rf_setup = { 0 };
    rf_setup.P_RF_DR_HIGH = 0;
    rf_setup.P_RF_DR_LOW = 1;
    rf_setup.P_RF_PWR = NRF24_RF_OUT_PWR_0DBM;
    nrf24_set_rf_setup(mNrf24, rf_setup);

    nrf24_set_rf_channel(mNrf24, channel);

    nrf24_setup_retr_t setup_retr;
    setup_retr.ARC = 15;
    setup_retr.ARD = 15;
    nrf24_set_setup_retr(mNrf24, setup_retr);

    nrf24_set_rx_payload_len(mNrf24, 0, NRF24_DEFAULT_PAYLOAD_LEN);

#ifdef ENABLE_DEBUG_LOG
    nrf24_dump_registers(mNrf24);
#endif
}

Radio::TaskResult Radio::task()
{
    static const int NRF_IRQ = GPIO_Pin_1;

    auto result = TaskResult::None;

    if (GPIO_ReadInputDataBit(GPIOC, NRF_IRQ) == 0)
    {
        DEBUG(Radio, "NRF interrupt");

        auto status = nrf24_get_status(mNrf24);

        nrf24_clear_interrupts(mNrf24);

        if (status.MAX_RT)
        {
            packetLost();
            result = TaskResult::PacketLost;
            mBusy = false;
        }

        if (status.RX_DR)
        {
            dataReceived();
            result = TaskResult::DataReceived;
            mBusy = false;
        }

        if (status.TX_DS)
        {
            dataSent();
            result = TaskResult::DataSent;
            mBusy = false;
        }

        printStats();
    }

    if (mBusy)
        result = TaskResult::Busy;

    return result;
}

void Radio::setRadioMode(
        const RadioMode mode,
        const uint8_t* txAddress,
        const uint8_t txAddressLen)
{
    DEBUG(Radio, "setting mode to %s", mode == RadioMode::PrimaryReceiver ? "PRX" : "PTX");

    nrf24_power_down(mNrf24);

    nrf24_config_t config = { 0 };
    config.EN_CRC = 1;
    config.PWR_UP = 1;

    if (mode == RadioMode::PrimaryReceiver)
    {
        nrf24_set_rx_address(mNrf24, 0, ReceiveAddress, sizeof(ReceiveAddress));
        config.PRIM_RX = 1;
    }
    else if (txAddress)
    {
        nrf24_set_tx_address(mNrf24, txAddress, txAddressLen);
        nrf24_set_rx_address(mNrf24, 0, txAddress, txAddressLen);
    }

    nrf24_set_config(mNrf24, config);
    nrf24_power_up(mNrf24);
}

void Radio::packetLost()
{
    DEBUG(Radio, "packet lost");
    ++mStats.lostPackets;

    setRadioMode(RadioMode::PrimaryReceiver);
}

void Radio::dataReceived()
{
    DEBUG(Radio, "data received");
    ++mStats.receivedPackets;

    while (1)
    {
        DEBUG(Radio, "reading incoming payload");

        auto fifo_status = nrf24_get_fifo_status(mNrf24);

        if (fifo_status.RX_EMPTY)
        {
            DEBUG(Radio, "RX FIFO empty");
            break;
        }

        protocol_msg_t msg;

        nrf24_read_rx_payload(mNrf24, msg.data, sizeof(protocol_msg_t));

        if (msg.msg_magic != PROTO_MSG_MAGIC)
        {
            DEBUG(Radio, "incoming protocol message is invalid");
            return;
        }

        // TODO: add incoming message to a queue instead of processing immediately
        processIncomingMessage(&msg);
    };
}

void Radio::dataSent()
{
    DEBUG(Radio, "data sent");
    ++mStats.sentPackets;

    setRadioMode(RadioMode::PrimaryReceiver);
}

void Radio::processIncomingMessage(protocol_msg_t* msg)
{
    DEBUG(Radio, "processing incoming message");

    // TODO maybe more than one message sould be stored
    mLastReceivedMessage = *msg;

#ifdef ENABLE_DEBUG_LOG
    print_protocol_message(msg);
#endif
}

void Radio::printStats()
{
    DEBUG(Radio, "sent: %u, recvd: %u, lost: %u\r\n",
            mStats.sentPackets,
            mStats.receivedPackets,
            mStats.lostPackets);
}

const protocol_msg_t& Radio::lastReceivedMessage() const
{
    return mLastReceivedMessage;
}
