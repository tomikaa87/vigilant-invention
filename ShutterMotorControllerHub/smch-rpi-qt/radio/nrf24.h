#ifndef NRF24_H
#define NRF24_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define NRF24_ENABLE_DUMP_REGISTERS
//#define NRF24_ENABLE_DEBUG_LOG

//
// Registers and masks
//

#define NRF24_REG_CONFIG                0x00
#define NRF24_CONFIG_MASK_RX_DR         (1 << 6)
#define NRF24_CONFIG_MASK_TX_DS         (1 << 5)
#define NRF24_CONFIG_MASK_MAX_RT        (1 << 4)
#define NRF24_CONFIG_EN_CRC             (1 << 3)
#define NRF24_CONFIG_CRCO               (1 << 2)
#define NRF24_CONFIG_PWR_UP             (1 << 1)
#define NRF24_CONFIG_PRIM_RX            (1 << 0)

#define NRF24_REG_EN_AA                 0x01
#define NRF24_EN_AA_ENAA_P5             (1 << 5)
#define NRF24_EN_AA_ENAA_P4             (1 << 4)
#define NRF24_EN_AA_ENAA_P3             (1 << 3)
#define NRF24_EN_AA_ENAA_P2             (1 << 2)
#define NRF24_EN_AA_ENAA_P1             (1 << 1)
#define NRF24_EN_AA_ENAA_P0             (1 << 0)

#define NRF24_REG_EN_RXADDR             0x02
#define NRF24_EN_RXADDR_ERX_P5          (1 << 5)
#define NRF24_EN_RXADDR_ERX_P4          (1 << 4)
#define NRF24_EN_RXADDR_ERX_P3          (1 << 3)
#define NRF24_EN_RXADDR_ERX_P2          (1 << 2)
#define NRF24_EN_RXADDR_ERX_P1          (1 << 1)
#define NRF24_EN_RXADDR_ERX_P0          (1 << 0)

#define NRF24_REG_SETUP_AW              0x03
#define NRF24_SETUP_AW_AW_MASK          0x03

#define NRF24_REG_SETUP_RETR            0x04
#define NRF24_SETUP_RETR_ARD_MASK       0xF0
#define NRF24_SETUP_RETR_ARC_MASK       0x0F

#define NRF24_REG_RF_CH                 0x05
#define NRF24_RF_CH_MASK                0x7F

#define NRF24_REG_RF_SETUP              0x06
#define NRF24_RF_SETUP_PLL_LOCK         (1 << 4)
#define NRF24_RF_SETUP_RF_DR            (1 << 3)
#define NRF24_RF_SETUP_RF_PWR_MASK      0x06
#define NRF24_RF_SETUP_LNA_HCURR        (1 << 0)

#define NRF24_REG_STATUS                0x07
#define NRF24_STATUS_RX_DR              (1 << 6)
#define NRF24_STATUS_TX_DS              (1 << 5)
#define NRF24_STATUS_MAX_RT             (1 << 4)
#define NRF24_STATUS_RX_P_NO_MASK       0x0E
#define NRF24_STATUS_TX_FULL            (1 << 0)
#define NRF24_RX_P_NO_RX_FIFO_EMPTY     0x07

#define NRF24_REG_OBSERVE_TX            0x08
#define NRF24_OBSERVE_TX_PLOS_CNT_MASK  0xF0
#define NRF24_OBSERVE_TX_ARC_CNT_MASK   0x0F

#define NRF24_REG_CD                    0x09
#define NRF24_CD_CD                     (1 << 0)

#define NRF24_REG_RPD                   0x09
#define NRF24_RPD_RPD                   (1 << 0)

#define NRF24_REG_RX_ADDR_P0            0x0A
#define NRF24_REG_RX_ADDR_P1            0x0B
#define NRF24_REG_RX_ADDR_P2            0x0C
#define NRF24_REG_RX_ADDR_P3            0x0D
#define NRF24_REG_RX_ADDR_P4            0x0E
#define NRF24_REG_RX_ADDR_P5            0x0F

#define NRF24_REG_TX_ADDR               0x10

#define NRF24_REG_RX_PW_P0              0x11
#define NRF24_RX_PW_P0_MASK             0x3F

#define NRF24_REG_RX_PW_P1              0x12
#define NRF24_RX_PW_P1_MASK             0x3F

#define NRF24_REG_RX_PW_P2              0x13
#define NRF24_RX_PW_P2_MASK             0x3F

#define NRF24_REG_RX_PW_P3              0x14
#define NRF24_RX_PW_P3_MASK             0x3F

#define NRF24_REG_RX_PW_P4              0x15
#define NRF24_RX_PW_P4_MASK             0x3F

#define NRF24_REG_RX_PW_P5              0x16
#define NRF24_RX_PW_P5_MASK             0x3F

#define NRF24_REG_FIFO_STATUS           0x17
#define NRF24_FIFO_STATUS_TX_REUSE      (1 << 6)
#define NRF24_FIFO_STATUS_TX_FULL       (1 << 5)
#define NRF24_FIFO_STATUS_TX_EMPTY      (1 << 4)
#define NRF24_FIFO_STATUS_RX_FULL       (1 << 1)
#define NRF24_FIFO_STATUS_RX_EMPTY      (1 << 0)

#define NRF24_REG_DYNPD                 0x1C
#define NRF24_DYNPD_DPL_P5              (1 << 5)
#define NRF24_DYNPD_DPL_P4              (1 << 4)
#define NRF24_DYNPD_DPL_P3              (1 << 3)
#define NRF24_DYNPD_DPL_P2              (1 << 2)
#define NRF24_DYNPD_DPL_P1              (1 << 1)
#define NRF24_DYNPD_DPL_P0              (1 << 0)

#define NRF24_REG_FEATURE               0x1D
#define NRF24_FEATURE_EN_DPL            (1 << 2)
#define NRF24_FEATURE_EN_ACK_PAY        (1 << 1)
#define NRF24_FEATURE_EN_DYN_ACK        (1 << 0)

//
// Commands
//

#define NRF24_CMD_R_REGISTER            0
#define NRF24_R_REGISTER_MASK           0x1F
#define NRF24_CMD_W_REGISTER            0x20
#define NRF24_W_REGISTER_MASK           0x1F
#define NRF24_CMD_R_RX_PAYLOAD          0x61
#define NRF24_CMD_W_TX_PAYLOAD          0xA0
#define NRF24_CMD_FLUSH_TX              0xE1
#define NRF24_CMD_FLUSH_RX              0xE2
#define NRF24_CMD_REUSE_TX_PL           0xE3
#define NRF24_CMD_ACTIVATE              0x50
#define NRF24_ACTIVATE_MAGIC            0x73
#define NRF24_CMD_R_RX_PL_WID           0x60
#define NRF24_CMD_W_ACK_PAYLOAD         0xA8
#define NRF24_W_ACK_PAYLOAD_MASK        0x07
#define NRF24_CMD_W_TX_PAYLOAD_NOACK    0xB0
#define NRF24_CMD_NOP                   0xFF

//
// Custom constants
//

#define NRF24_DEFAULT_PAYLOAD_LEN       32

//
// Types
//

typedef enum
{
    NRF24_LOW = 0,
    NRF24_HIGH
} nrf24_state_t;

typedef void (*nrf24_set_pin_func)(nrf24_state_t state);
typedef uint8_t (*nrf24_spi_exchange_func)(uint8_t data);

typedef struct
{
    nrf24_set_pin_func set_ce;
    nrf24_set_pin_func set_csn;
    nrf24_spi_exchange_func spi_exchange;
} nrf24_t;

typedef enum
{
    NRF24_DR_1MBPS = 0, NRF24_DR_2MBPS
} nrf24_data_rate_t;

typedef enum
{
    NRF24_RF_OUT_PWR_MINUS_18DBM = 0,
    NRF24_RF_OUT_PWR_MINUS_12DBM,
    NRF24_RF_OUT_PWR_MINUS_6DBM,
    NRF24_RF_OUT_PWR_0DBM
} nrf24_rf_out_pwr_t;

typedef enum
{
    NRF24_LNA_GAIN_STEP1 = 0,
    NRF24_LNA_GAIN_STEP2
} nrf24_lna_gain_t;

typedef union
{
    struct
    {
        unsigned PRIM_RX :1;
        unsigned PWR_UP :1;
        unsigned CRCO :1;
        unsigned EN_CRC :1;
        unsigned MASK_MAX_RT :1;
        unsigned MASK_TX_DS :1;
        unsigned MASK_RX_DR :1;
        unsigned reserved :1;
    };

    uint8_t value;
} nrf24_config_t;

typedef union
{
    struct
    {
        unsigned ENAA_P0 :1;
        unsigned ENAA_P1 :1;
        unsigned ENAA_P2 :1;
        unsigned ENAA_P3 :1;
        unsigned ENAA_P4 :1;
        unsigned ENAA_P5 :1;
        unsigned reserved :2;
    };

    uint8_t value;
} nrf24_en_aa_t;

typedef union
{
    struct
    {
        unsigned EN_P0 :1;
        unsigned EN_P1 :1;
        unsigned EN_P2 :1;
        unsigned EN_P3 :1;
        unsigned EN_P4 :1;
        unsigned EN_P5 :1;
        unsigned reserved :2;
    };

    uint8_t value;
} nrf24_en_rxaddr_t;

typedef enum
{
    NRF24_AW_3_BYTES = 1,
    NRF24_AW_4_BYTES,
    NRF24_AW_5_BYTES,
} nrf24_aw_t;

typedef union
{
    struct
    {
        nrf24_aw_t AW :2;
        unsigned reserved :6;
    };

    uint8_t value;
} nrf24_setup_aw_t;

typedef union
{
    struct
    {
        unsigned RF_CH :7;
        unsigned reserved :1;
    };

    uint8_t value;
} nrf24_rf_ch_t;

typedef union
{
    struct
    {
        unsigned TX_FULL :1;
        unsigned RX_P_NO :3;
        unsigned MAX_RT :1;
        unsigned TX_DS :1;
        unsigned RX_DR :1;
        unsigned reserved :1;
    };

    uint8_t value;
} nrf24_status_t;

typedef union
{
    struct
    {
        unsigned RX_EMPTY :1;
        unsigned RX_FULL :1;
        unsigned reserved1 :2;
        unsigned TX_EMPTY :1;
        unsigned TX_FULL :1;
        unsigned TX_REUSE :1;
        unsigned reserved2 :1;
    };

    uint8_t value;
} nrf24_fifo_status_t;

typedef union
{
    struct
    {
        unsigned DPL_P0 :1;
        unsigned DPL_P1 :1;
        unsigned DPL_P2 :1;
        unsigned DPL_P3 :1;
        unsigned DPL_P4 :1;
        unsigned DPL_P5 :1;
        unsigned reserved :2;
    };

    uint8_t value;
} nrf24_dynpd_t;

typedef union
{
    struct
    {
        unsigned EN_DYN_ACK :1;
        unsigned EN_ACK_PAY :1;
        unsigned EN_DPL :1;
        unsigned reserved :5;
    };

    uint8_t value;
} nrf24_feature_t;

typedef union
{
    struct
    {
        unsigned ARC_CTN :4;
        unsigned PLOS_CNT :4;
    };

    uint8_t value;
} nrf24_observe_tx_t;

typedef union
{
    struct
    {
        unsigned RPD :1;
        unsigned reserved :7;
    };

    uint8_t value;
} nrf24_rpd_t;

typedef union
{
    struct
    {
        unsigned LNA_HCURR :1;
        unsigned RF_PWR :2;
        unsigned RF_DR :1;
        unsigned PLL_LOCK :1;
        unsigned reserved :3;
    };

    // New bit definitions for NRF24L01+
    struct
    {
        unsigned P_obsolete :1;
        unsigned P_RF_PWR :2;
        unsigned P_RF_DR_HIGH :1;
        unsigned P_PLL_LOCK :1;
        unsigned P_RF_DR_LOW :1;
        unsigned P_reserved :1;
        unsigned P_CONT_WAVE :1;
    };

    uint8_t value;
} nrf24_rf_setup_t;

typedef union
{
    struct
    {
        unsigned ARC :4;
        unsigned ARD :4;
    };

    uint8_t value;
} nrf24_setup_retr_t;

//
// Functions
//

void nrf24_init(nrf24_t* radio, nrf24_set_pin_func set_ce_func,
        nrf24_set_pin_func set_csn_func,
        nrf24_spi_exchange_func spi_exchange_func);

void nrf24_set_config(nrf24_t* radio, nrf24_config_t config);

nrf24_config_t nrf24_get_config(nrf24_t* radio);

void nrf24_set_enable_auto_ack(nrf24_t* radio, nrf24_en_aa_t en_aa);

nrf24_en_aa_t nrf24_get_enable_auto_ack(nrf24_t* radio);

void nrf24_set_enabled_rx_addr(nrf24_t* radio, nrf24_en_rxaddr_t en_rxaddr);

nrf24_en_rxaddr_t nrf24_get_enabled_rx_addr(nrf24_t* radio);

void nrf24_set_setup_aw(nrf24_t* radio, nrf24_setup_aw_t setup_aw);

nrf24_setup_aw_t nrf24_get_setup_aw(nrf24_t* radio);

void nrf24_set_rx_address(nrf24_t* radio, uint8_t pipe, const uint8_t* addr,
        uint8_t len);

void nrf24_get_rx_address(nrf24_t* radio, uint8_t pipe, uint8_t* addr,
        uint8_t len);

void nrf24_set_tx_address(nrf24_t* radio, const uint8_t* addr, uint8_t len);

void nrf24_get_tx_address(nrf24_t* radio, uint8_t* addr, uint8_t len);

void nrf24_set_rf_channel(nrf24_t* radio, uint8_t channel);

void nrf24_set_rf_ch(nrf24_t* radio, nrf24_rf_ch_t rf_ch);

nrf24_rf_ch_t nrf24_get_rf_ch(nrf24_t* radio);

void nrf24_setup_rf(nrf24_t* radio, nrf24_data_rate_t data_rate,
        nrf24_rf_out_pwr_t output_power, nrf24_lna_gain_t lna_gain);

void nrf24_set_rf_setup(nrf24_t* radio, nrf24_rf_setup_t rf_setup);

nrf24_rf_setup_t nrf24_get_rf_setup(nrf24_t* radio);

void nrf24_set_setup_retr(nrf24_t* radio, nrf24_setup_retr_t setup_retr);

nrf24_setup_retr_t nrf24_get_setup_retr(nrf24_t* radio);

void nrf24_set_status(nrf24_t* radio, nrf24_status_t status);

nrf24_status_t nrf24_get_status(nrf24_t* radio);

nrf24_observe_tx_t nrf24_get_observe_tx(nrf24_t* radio);

nrf24_rpd_t nrf24_get_rpd(nrf24_t* radio);

uint8_t nrf24_get_rx_payload_len(nrf24_t* radio, uint8_t pipe);

void nrf24_set_rx_payload_len(nrf24_t* radio, uint8_t pipe, uint8_t len);

nrf24_fifo_status_t nrf24_get_fifo_status(nrf24_t* radio);

void nrf24_set_dyn_payload(nrf24_t* radio, nrf24_dynpd_t dynpd);

nrf24_dynpd_t nrf24_get_dyn_payload(nrf24_t* radio);

void nrf24_set_feature(nrf24_t* radio, nrf24_feature_t feature);

nrf24_feature_t nrf24_get_feature(nrf24_t* radio);

void nrf24_write_tx_payload(nrf24_t* radio, const uint8_t* buf, uint8_t len);

void nrf24_read_rx_payload(nrf24_t* radio, uint8_t* buf, uint8_t len);

void nrf24_clear_interrupts(nrf24_t* radio);

void nrf24_setup_primary_receiver(nrf24_t* radio);

void nrf24_transmit_data(nrf24_t* radio, const uint8_t* buf, uint8_t len);

void nrf24_power_up_rx(nrf24_t* radio);

void nrf24_flush_rx(nrf24_t* radio);

void nrf24_flush_tx(nrf24_t* radio);

void nrf24_power_up(nrf24_t* radio);

void nrf24_power_down(nrf24_t* radio);

//
// Utility functions
//

#ifdef NRF24_ENABLE_DUMP_REGISTERS

void nrf24_dump_registers(nrf24_t* radio);

void nrf24_dump_status(nrf24_t* radio);

void nrf24_dump_fifo_status(nrf24_t* radio);

void nrf24_dump_observe_tx(nrf24_t* radio);

#endif

//
// Low level functions
//

uint8_t nrf24_read_register(nrf24_t* radio, uint8_t reg);

void nrf24_write_register(nrf24_t* radio, uint8_t reg, uint8_t value);

void nrf24_read_multi_byte_register(nrf24_t* radio, uint8_t reg, uint8_t* buf,
        uint8_t len);

void nrf24_write_multi_byte_register(nrf24_t* radio, uint8_t reg,
        const uint8_t* buf, uint8_t len);

void nrf24_send_command(nrf24_t* radio, uint8_t command);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // NRF24_H
