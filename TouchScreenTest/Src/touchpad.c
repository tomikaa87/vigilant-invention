/*
 * touchpad.c
 *
 *  Created on: 2018. dec. 30.
 *      Author: tomikaa
 */


#include "touchpad.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "tpfilter.h"
#include "graphics.h"

#include <stdint.h>
#include <stdio.h>


#define AVG_FILTER_COUNT    10


static void standby()
{
    GPIO_InitTypeDef gpio = { 0 };

    // XL -> GND
    gpio.Pin = TP_XL_Pin;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TP_XL_GPIO_Port, &gpio);
    HAL_GPIO_WritePin(TP_XL_GPIO_Port, TP_XL_Pin, GPIO_PIN_RESET);

    // XR -> Hi-Z
    gpio.Pin = TP_XR_Pin;
    gpio.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(TP_XR_GPIO_Port, &gpio);

    // YU -> Hi-Z
    gpio.Pin = TP_YU_Pin;
    gpio.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(TP_YU_GPIO_Port, &gpio);

    // YD -> Hi-Z
    gpio.Pin = TP_YD_Pin;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(TP_YD_GPIO_Port, &gpio);
}

static uint32_t readADCX()
{
    GPIO_InitTypeDef gpio = { 0 };

    // XL -> GND
    gpio.Pin = TP_XL_Pin;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TP_XL_GPIO_Port, &gpio);
    HAL_GPIO_WritePin(TP_XL_GPIO_Port, TP_XL_Pin, GPIO_PIN_RESET);

    // XR -> Vcc
    gpio.Pin = TP_XR_Pin;
    HAL_GPIO_Init(TP_XR_GPIO_Port, &gpio);
    HAL_GPIO_WritePin(TP_XR_GPIO_Port, TP_XR_Pin, GPIO_PIN_SET);

    // YU -> ADC
    gpio.Pin = TP_YU_Pin;
    gpio.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(TP_YU_GPIO_Port, &gpio);

    // YD -> Hi-Z
    gpio.Pin = TP_YD_Pin;
    gpio.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(TP_YD_GPIO_Port, &gpio);

    // Read YU ADC
    uint32_t sum = 0;
    for (uint8_t i = 0; i < AVG_FILTER_COUNT; ++i)
    {
        HAL_ADC_Start(&hadc2);
        HAL_ADC_PollForConversion(&hadc2, 250);
        sum += HAL_ADC_GetValue(&hadc2);
    }

    return sum / AVG_FILTER_COUNT;
}

static uint16_t readADCY()
{
    GPIO_InitTypeDef gpio = { 0 };

    // XL -> ADC
    gpio.Pin = TP_XL_Pin;
    gpio.Mode = GPIO_MODE_ANALOG;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TP_XL_GPIO_Port, &gpio);

    // XR -> Hi-Z
    gpio.Pin = TP_XR_Pin;
    gpio.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(TP_XR_GPIO_Port, &gpio);

    // YU -> GND
    gpio.Pin = TP_YU_Pin;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(TP_YU_GPIO_Port, &gpio);
    HAL_GPIO_WritePin(TP_YU_GPIO_Port, TP_YU_Pin, GPIO_PIN_RESET);

    // YD -> Vcc
    gpio.Pin = TP_YD_Pin;
    HAL_GPIO_Init(TP_YD_GPIO_Port, &gpio);
    HAL_GPIO_WritePin(TP_YD_GPIO_Port, TP_YD_Pin, GPIO_PIN_SET);

    // Read XL ADC
    uint32_t sum = 0;
    for (uint8_t i = 0; i < AVG_FILTER_COUNT; ++i)
    {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 250);
        sum += HAL_ADC_GetValue(&hadc1);
    }

    return sum / AVG_FILTER_COUNT;
}

void Touchpad_Init()
{
    standby();
}

void Touchpad_Task()
{
    static TPFilter_Channel xChannel;
    static TPFilter_Channel yChannel;
    static bool initialized = false;

    if (!initialized)
    {
        initialized = true;

        TPFilter_InitChannel(&xChannel);
        TPFilter_InitChannel(&yChannel);
    }

    uint16_t xAdc = readADCX();
    standby();
    uint16_t yAdc = readADCY();
    standby();

    bool pressed = xAdc < 3700;

    uint16_t xFiltered = TPFilter_InputSample(&xChannel, xAdc, pressed);
    uint16_t yFiltered = TPFilter_InputSample(&yChannel, yAdc, pressed);

    // Detect touch
    if (TPFilter_IsPressed(&xChannel))
    {
        static uint16_t xMin = 5000, xMax = 0, yMin = 5000, yMax = 0;

//        uint16_t xFiltered = xAdc;
//        TPFilter_Input(&xFiltered, &yAdc);

        if (xFiltered < xMin)
            xMin = xFiltered;
        if (xFiltered > xMax)
            xMax = xFiltered;

        if (yFiltered < yMin)
            yMin = yFiltered;
        if (yFiltered > yMax)
            yMax = yFiltered;

        uint8_t x = 240 - (xFiltered - xMin) * 240 / (xMax - xMin);
        uint8_t y = 160 - (yFiltered - yMin) * 160 / (yMax - yMin);

#if 0
        printf("X = %3d (%4d, %4d-%4d), Y = %3d (%4d, %4d-%4d)\r\n",
                x, xFiltered, xMin, xMax, y, yFiltered, yMin, yMax);
#endif

        Graphics_DrawCursor(x, y);
    }
//    else
//    {
//        HAL_Delay(50);
//    }
}
