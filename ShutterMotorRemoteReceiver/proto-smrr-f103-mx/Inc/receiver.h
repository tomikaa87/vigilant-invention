/*
 * receiver.h
 *
 *  Created on: 2018. máj. 5.
 *      Author: tkarpati
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

#include "stm32f1xx_hal.h"

void receiver_init(SPI_HandleTypeDef* hspi);
void receiver_task();

#endif /* RECEIVER_H_ */
