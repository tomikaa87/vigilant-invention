/*
 * receiver.c
 *
 *  Created on: 2018. máj. 5.
 *      Author: tkarpati
 */

#include "receiver.h"
#include "radio_protocol.h"
#include "nrf24.h"
#include "opto_controller.h"

// #define RECEIVER_ENABLE_DEBUG_LOG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void nrf_set_ce(nrf24_state_t state);
static void nrf_set_csn(nrf24_state_t state);
static uint8_t nrf_spi_exchange(uint8_t data);

static void configure();
static void switch_to_prx();
static void switch_to_ptx();

static void process_message(const protocol_msg_t* msg);

static void send_message_to_hub(const protocol_msg_t* msg);
static void send_result_to_hub(protocol_result_code_t result_code);

static void process_command_request(const protocol_msg_t* msg);
static void process_read_status_request(const protocol_msg_t* msg);
static void process_read_temperature_request(const protocol_msg_t* msg);

static void packet_lost();
static void data_received();
static void data_sent();

static void store_cmd(protocol_cmd_t cmd);

static void reset_plos_cnt();

static uint8_t ReceiveAddress[] = { 'S', 'M', 'R', 'R', 0 };
static const uint8_t TransmitAddress[] = { 'S', 'M', 'R', 'H', '1' };
static const char FirmwareVersion[] = "1.0.2";

static struct
{
    nrf24_t radio;
    SPI_HandleTypeDef* hspi;
    uint8_t channel;

    protocol_cmd_t last_cmds[10];
    uint8_t last_cmds_ptr;
} recv;

void receiver_init(SPI_HandleTypeDef* hspi)
{
#ifdef RECEIVER_ENABLE_DEBUG_LOG
    printf("Initializing Receiver\r\n");
#endif

    memset(recv.last_cmds, 0, sizeof(recv.last_cmds));
    recv.last_cmds_ptr = 0;

    // Read LSB of ReceiveAddress from Option Byte 0
    uint8_t address_lsb = HAL_FLASHEx_OBGetUserData(OB_DATA_ADDRESS_DATA0);
    ReceiveAddress[4] = address_lsb;

    // Read NRF24 channel number from Option Byte 1
    uint8_t channel = HAL_FLASHEx_OBGetUserData(OB_DATA_ADDRESS_DATA1);

#ifdef RECEIVER_DEBUG_CHANNEL_OVERRIDE
    channel = RECEIVER_DEBUG_CHANNEL_OVERRIDE;
#endif

    recv.channel = channel & NRF24_RF_CH_MASK;

#ifdef RECEIVER_ENABLE_DEBUG_LOG
    printf(
            "Configuration: receive address LSB = 0x%x, channel = %d (%d MHz)\r\n",
            address_lsb, recv.channel, recv.channel + 2400);

    if (channel != recv.channel)
    {
        printf("Invalid channel in Option Byte 1: 0x%x\r\n", channel);
    }
#endif

    recv.hspi = hspi;

    nrf24_init(&recv.radio, nrf_set_ce, nrf_set_csn, nrf_spi_exchange);
    nrf24_power_down(&recv.radio);

    HAL_Delay(100);

    configure();
}

void receiver_task()
{
    if (HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port, NRF_IRQ_Pin) == 0)
    {
        nrf24_status_t status = nrf24_get_status(&recv.radio);

#ifdef RECEIVER_ENABLE_DEBUG_LOG
        printf("Status: %x\r\n", status.value);
#endif

        nrf24_clear_interrupts(&recv.radio);

        if (status.MAX_RT)
            packet_lost();

        if (status.RX_DR)
            data_received();

        if (status.TX_DS)
            data_sent();
    }
}

static void nrf_set_ce(nrf24_state_t state)
{
    HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin,
            state == NRF24_HIGH ? GPIO_PIN_SET : GPIO_PIN_RESET);
    asm("nop");
}

static void nrf_set_csn(nrf24_state_t state)
{
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin,
            state == NRF24_HIGH ? GPIO_PIN_SET : GPIO_PIN_RESET);
    asm("nop");
}

static uint8_t nrf_spi_exchange(uint8_t data)
{
    uint8_t rx_data = 0;
    HAL_SPI_TransmitReceive(recv.hspi, &data, &rx_data, 1, 5000);
    return rx_data;
}

static void configure()
{
#ifdef RECEIVER_ENABLE_DEBUG_LOG
    printf("Configuring Receiver\r\n");
#endif

    nrf24_setup_aw_t setup_aw;
    setup_aw.value = 0;
    setup_aw.AW = NRF24_AW_5_BYTES;
    nrf24_set_setup_aw(&recv.radio, setup_aw);

#ifdef RECEIVER_ENABLE_DEBUG_LOG
    nrf24_setup_aw_t real_setup_aw = nrf24_get_setup_aw(&recv.radio);
    if (setup_aw.value != real_setup_aw.value)
    {
        printf("SETUP_AW value mismatch\r\n");
    }
#endif

    nrf24_rf_setup_t rf_setup = { 0 };
    rf_setup.P_RF_DR_HIGH = 0;
    rf_setup.P_RF_DR_LOW = 1;
    nrf24_set_rf_setup(&recv.radio, rf_setup);

    nrf24_set_rf_channel(&recv.radio, recv.channel);

    nrf24_setup_retr_t setup_retr = { 0 };
    setup_retr.ARC = 15;
    setup_retr.ARD = 15;
    nrf24_set_setup_retr(&recv.radio, setup_retr);

    nrf24_set_rx_payload_len(&recv.radio, 0, NRF24_DEFAULT_PAYLOAD_LEN);

    // To be able to send response to the HUB
    nrf24_set_tx_address(&recv.radio, TransmitAddress, sizeof(TransmitAddress));

    switch_to_prx();
}

static void switch_to_prx()
{
#ifdef RECEIVER_ENABLE_DEBUG_LOG
    printf("Switching to PRX\r\n");
#endif

    nrf24_power_down(&recv.radio);

    nrf24_set_rx_address(&recv.radio, 0, ReceiveAddress,
            sizeof(ReceiveAddress));

    reset_plos_cnt();

    nrf24_config_t config = { 0 };
    config.EN_CRC = 1;
    config.PRIM_RX = 1;
    config.PWR_UP = 1;
    nrf24_set_config(&recv.radio, config);

    nrf24_power_up(&recv.radio);
}

static void switch_to_ptx()
{
#ifdef RECEIVER_ENABLE_DEBUG_LOG
    printf("Switching to PTX\r\n");
#endif

    nrf24_power_down(&recv.radio);

    nrf24_set_rx_address(&recv.radio, 0, TransmitAddress,
            sizeof(TransmitAddress));

    reset_plos_cnt();

    nrf24_config_t config = { 0 };
    config.EN_CRC = 1;
    config.PWR_UP = 1;
    nrf24_set_config(&recv.radio, config);
}

static void process_message(const protocol_msg_t* msg)
{
#ifdef RECEIVER_ENABLE_DEBUG_LOG
    printf("Processing message:\r\n");

    print_protocol_message(msg);
#endif

    switch (msg->msg_type)
    {
        case PROTO_MSG_COMMAND:
            process_command_request(msg);
            break;

        case PROTO_MSG_READ_STATUS:
            process_read_status_request(msg);
            break;

        case PROTO_MSG_READ_TEMPERATURE:
            process_read_temperature_request(msg);
            break;

        default:
            send_result_to_hub(PROTO_RESULT_UNKNOWN_MSG_TYPE);
            break;
    }
}

static void send_message_to_hub(const protocol_msg_t* msg)
{
#ifdef RECEIVER_ENABLE_DEBUG_LOG
    printf("Sending message to the HUB:\r\n");

    print_protocol_message(msg);
#endif

    switch_to_ptx();

    nrf24_transmit_data(&recv.radio, msg->data, sizeof(protocol_msg_t));
    nrf24_power_up(&recv.radio);
}

static void send_result_to_hub(protocol_result_code_t result_code)
{
    protocol_msg_t response;
    protocol_msg_init(&response);

    response.msg_type = PROTO_MSG_RESULT;
    response.payload.result.result_code = result_code;

    send_message_to_hub(&response);
}

static void process_command_request(const protocol_msg_t* msg)
{
    store_cmd(msg->payload.command);

    switch (msg->payload.command)
    {
        case PROTO_CMD_SHUTTER_1_UP:
            opto_pulse(OPTO_A);
            break;

        case PROTO_CMD_SHUTTER_1_DOWN:
            opto_pulse(OPTO_B);
            break;

        case PROTO_CMD_SHUTTER_2_UP:
            opto_pulse(OPTO_C);
            break;

        case PROTO_CMD_SHUTTER_2_DOWN:
            opto_pulse(OPTO_D);
            break;

        default:
            send_result_to_hub(PROTO_RESULT_NO_SUCH_COMMAND);
            return;
    }

    send_result_to_hub(PROTO_RESULT_OK);
}

static void process_read_status_request(const protocol_msg_t* msg)
{
    protocol_msg_t response;
    protocol_msg_init(&response);

    response.msg_type = PROTO_MSG_READ_STATUS_RESULT;

    memcpy(response.payload.status.firmware_ver, FirmwareVersion,
            sizeof(FirmwareVersion));
    memcpy(response.payload.status.last_commands, recv.last_cmds,
            sizeof(recv.last_cmds));

    send_message_to_hub(&response);
}

static void process_read_temperature_request(const protocol_msg_t* msg)
{
    send_result_to_hub(PROTO_RESULT_UNSUPPORTED_COMMAND);
}

static void packet_lost()
{
    nrf24_observe_tx_t otx = nrf24_get_observe_tx(&recv.radio);

#ifdef RECEIVER_ENABLE_DEBUG_LOG
    printf("Packet lost. Count: %u\r\n", otx.PLOS_CNT);
#endif

    // PLOS_CNT must be reset to be able to transmit again
    if (otx.PLOS_CNT >= 15)
    {
#ifdef RECEIVER_ENABLE_DEBUG_LOG
        printf("Max packet loss count reached, resetting\r\n");
#endif
    }

    switch_to_prx();
}

static void data_received()
{
#ifdef RECEIVER_ENABLE_DEBUG_LOG
    printf("Data received\r\n");
#endif

    while (1)
    {
#ifdef RECEIVER_ENABLE_DEBUG_LOG
        printf("Reading incoming payload\r\n");
#endif

        nrf24_fifo_status_t fifo_status = nrf24_get_fifo_status(&recv.radio);

        if (fifo_status.RX_EMPTY)
            break;

        protocol_msg_t msg;

        nrf24_read_rx_payload(&recv.radio, msg.data, sizeof(protocol_msg_t));

        if (msg.msg_magic != PROTO_MSG_MAGIC)
        {
#ifdef RECEIVER_ENABLE_DEBUG_LOG
            printf("Protocol magic mismatch\r\n");
#endif
            return;
        }

        process_message(&msg);
    };
}

static void data_sent()
{
    switch_to_prx();
}

static void store_cmd(protocol_cmd_t cmd)
{
    if (recv.last_cmds_ptr < sizeof(recv.last_cmds))
    {
        recv.last_cmds[recv.last_cmds_ptr] = cmd;
        ++recv.last_cmds_ptr;
    }
    else
    {
        for (uint8_t i = 1; i < sizeof(recv.last_cmds); ++i)
        {
            recv.last_cmds[i - 1] = recv.last_cmds[i];
        }

        recv.last_cmds[sizeof(recv.last_cmds) - 1] = cmd;
    }
}

static void reset_plos_cnt()
{
#ifdef RECEIVER_ENABLE_DEBUG_LOG
    printf("Resetting packet loss counter\r\n");
#endif

    // According to the documentation, setting RF_CH resets PLOS_CNT
    nrf24_set_rf_channel(&recv.radio, recv.channel);
}
