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

#define LCD_SET_NRD() HAL_GPIO_WritePin(LCD_nRD_GPIO_Port, LCD_nRD_Pin, GPIO_PIN_SET)
#define LCD_RESET_NRD() HAL_GPIO_WritePin(LCD_nRD_GPIO_Port, LCD_nRD_Pin, GPIO_PIN_RESET)

#define LCD_SET_CSB() HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
#define LCD_RESET_CSB() HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)

#define LCD_SET_NWR() HAL_GPIO_WritePin(LCD_nWR_GPIO_Port, LCD_nWR_Pin, GPIO_PIN_SET)
#define LCD_RESET_NWR() HAL_GPIO_WritePin(LCD_nWR_GPIO_Port, LCD_nWR_Pin, GPIO_PIN_RESET)

#define LCD_SET_RST() HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)
#define LCD_RESET_RST() HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)

#define LCD_WRITE_PORT(X) *(volatile uint8_t*)&GPIOA->ODR = X
#define LCD_READ_PORT() (GPIOA->IDR & 0xff)

#define LCD_DELAY_MS(X) HAL_Delay(X)

#define LCD_SET_PORT_OUTPUT() { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = LCD_D0_Pin|LCD_D1_Pin|LCD_D2_Pin|LCD_D3_Pin |LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin; \
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; \
    GPIO_InitStruct.Pull = GPIO_NOPULL; \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; \
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); \
}

#define LCD_SET_PORT_INPUT() { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = LCD_D0_Pin|LCD_D1_Pin|LCD_D2_Pin|LCD_D3_Pin |LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin; \
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
    GPIO_InitStruct.Pull = GPIO_NOPULL; \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; \
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); \
}

#endif /* LCDCONFIG_H_ */
