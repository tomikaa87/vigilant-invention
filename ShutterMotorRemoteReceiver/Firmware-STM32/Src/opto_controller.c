/*
 * opto_controller.c
 *
 *  Created on: 2018. máj. 5.
 *      Author: tkarpati
 */

#include "opto_controller.h"
#include "stm32f0xx_hal.h"

static GPIO_TypeDef* const OPTO_PORTS[] =
{
		OPTO_PORT_A_GPIO_Port,
		OPTO_PORT_B_GPIO_Port,
		OPTO_PORT_C_GPIO_Port,
		OPTO_PORT_D_GPIO_Port
};

static uint16_t const OPTO_PINS[] =
{
		OPTO_PORT_A_Pin,
		OPTO_PORT_B_Pin,
		OPTO_PORT_C_Pin,
		OPTO_PORT_D_Pin,
};

void opto_set_state(const opto_port_t port, const opto_state_t state)
{
	HAL_GPIO_WritePin(OPTO_PORTS[port], OPTO_PINS[port], state == OPTO_ENABLED ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void opto_pulse(const opto_port_t port)
{
	opto_set_state(port, OPTO_ENABLED);
	HAL_Delay(200);
	opto_set_state(port, OPTO_DISABLED);
}
