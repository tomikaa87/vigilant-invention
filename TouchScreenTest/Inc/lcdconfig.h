/*
 * lcdconfig.h
 *
 *  Created on: 2018. dec. 30.
 *      Author: tomikaa
 */

#ifndef LCDCONFIG_H_
#define LCDCONFIG_H_

#include "main.h"

#define LCD_SET_A0() HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, GPIO_PIN_SET)
#define LCD_RESET_A0() HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, GPIO_PIN_RESET)

#define LCD_SET_E() HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_SET)
#define LCD_RESET_E() HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_RESET)

#define LCD_SET_CSB() HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
#define LCD_RESET_CSB() HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)

#define LCD_SET_RW() HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_SET)
#define LCD_RESET_RW() HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET)

#define LCD_SET_RST() HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)
#define LCD_RESET_RST() HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)

#define LCD_WRITE_PORT(X) *(volatile uint8_t*)&GPIOA->ODR = X
#define LCD_READ_PORT() (GPIOA->ODR & 0xff)

#define LCD_DELAY_MS(X) HAL_Delay(X)

#endif /* LCDCONFIG_H_ */
