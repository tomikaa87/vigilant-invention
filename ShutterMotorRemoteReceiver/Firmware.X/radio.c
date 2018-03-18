#include "radio.h"
#include "nrf24l01plus/nrf24.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/spi.h"

#include <xc.h>

void radio_init()
{
	nrf24_init();
}

// Platform-dependent functions for NRF24 driver

void nrf24_setupPins()
{
}

void nrf24_ce_digitalWrite(uint8_t state)
{
	IO_NRF_CE_LAT = state ? 1u : 0u;
}

void nrf24_csn_digitalWrite(uint8_t state)
{
	IO_NRF_CSN_LAT = state ? 1u : 0u;
}

uint8_t nrf24_spi_exchange(uint8_t data)
{
	return SPI_Exchange8bit(data);
}

