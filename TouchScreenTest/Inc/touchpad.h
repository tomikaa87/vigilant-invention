/*
 * touchpad.h
 *
 *  Created on: 2018. dec. 30.
 *      Author: tomikaa
 */

#ifndef TOUCHPAD_H_
#define TOUCHPAD_H_

#include <stdbool.h>
#include <stdint.h>

void Touchpad_Init();
bool Touchpad_Task(uint8_t* x, uint8_t* y, bool* pressed);

#endif /* TOUCHPAD_H_ */
