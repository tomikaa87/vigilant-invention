/*
 * opto_controller.h
 *
 *  Created on: 2018. máj. 5.
 *      Author: tkarpati
 */

#ifndef OPTO_CONTROLLER_H_
#define OPTO_CONTROLLER_H_

typedef enum
{
	OPTO_A,
	OPTO_B,
	OPTO_C,
	OPTO_D
} opto_port_t;

typedef enum
{
	OPTO_DISABLED,
	OPTO_ENABLED
} opto_state_t;

void opto_set_state(const opto_port_t port, const opto_state_t state);
void opto_pulse(const opto_port_t port);

#endif /* OPTO_CONTROLLER_H_ */
