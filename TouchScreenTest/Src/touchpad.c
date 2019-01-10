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


extern ADC_HandleTypeDef hadc1;


#define AVG_FILTER_COUNT    10


static uint16_t g_adcResults[2] = { 0 };
static volatile bool g_adcConvCompleted = false;


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
        g_adcConvCompleted = false;
        while (!g_adcConvCompleted);

        sum += g_adcResults[1];
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
        g_adcConvCompleted = false;
        while (!g_adcConvCompleted);

        sum += g_adcResults[0];
    }

    return sum / AVG_FILTER_COUNT;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    UNUSED(hadc);
    g_adcConvCompleted = true;
}

void Touchpad_Init()
{
    HAL_StatusTypeDef status;

    status = HAL_ADC_Start_DMA(&hadc1,
                               (uint32_t*)g_adcResults,
                               sizeof(g_adcResults) / sizeof(g_adcResults[0]));

    if (status != HAL_OK)
    {
        printf("Touchpad: failed to start ADC in DMA mode\r\n");
        return;
    }

    printf("Touchpad initialized\r\n");

    standby();
}

bool Touchpad_Task(uint8_t* x, uint8_t* y, bool* pressed)
{
    static TPFilter_Channel xChannel;
    static TPFilter_Channel yChannel;
    static bool initialized = false;
    static bool lastPressed = false;
    static uint8_t lastX = 0;
    static uint8_t lastY = 0;

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

    bool touchDetected = xAdc < 3700;

    uint16_t xFiltered = TPFilter_InputSample(&xChannel, xAdc, touchDetected);
    uint16_t yFiltered = TPFilter_InputSample(&yChannel, yAdc, touchDetected);

    if (TPFilter_IsPressedDebounced(&xChannel))
    {
        bool changed = false;

        if (!lastPressed)
        {
            lastPressed = true;
            changed = true;
        }

        static uint16_t xMin = 5000, xMax = 0, yMin = 5000, yMax = 0;

        if (xFiltered < xMin)
            xMin = xFiltered;
        if (xFiltered > xMax)
            xMax = xFiltered;

        if (yFiltered < yMin)
            yMin = yFiltered;
        if (yFiltered > yMax)
            yMax = yFiltered;

        uint8_t currentX = 240 - (xFiltered - xMin) * 240 / (xMax - xMin);
        uint8_t currentY = 160 - (yFiltered - yMin) * 160 / (yMax - yMin);

        if (currentX != lastX || currentY != lastY)
        {
            lastX = currentX;
            lastY = currentY;
            changed = true;
        }

        if (changed)
        {
            *x = currentX;
            *y = currentY;
            *pressed = true;
        }
#if 0
        printf("X = %3d (%4d, %4d-%4d), Y = %3d (%4d, %4d-%4d)\r\n",
               x, xFiltered, xMin, xMax, y, yFiltered, yMin, yMax);
#endif

        return changed;
    }
    else if (lastPressed)
    {
        lastPressed = false;

        *x = lastX;
        *y = lastY;
        *pressed = false;

        return true;
    }

    return false;
}
