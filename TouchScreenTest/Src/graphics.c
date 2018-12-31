/*
 * graphics.c
 *
 *  Created on: 2018. dec. 30.
 *      Author: tomikaa
 */


#include "graphics.h"
#include "lcd.h"

static uint8_t g_buffer[240 / 3 * 160] = { 0, };


void Graphics_DrawCursor(uint8_t x, uint8_t y)
{
    static uint8_t lastX = 0;
    static uint8_t lastY = 0;

    if (lastX == x && lastY == y)
        return;

    lastX = x;
    lastY = y;

    if (x < 10 && y < 10)
        LCD_Clear();

    LCD_SetAddress(x / 2, y);
    LCD_WriteDisplayData(0xff);
}

void Graphics_UpdateScreen()
{
    LCD_WriteDisplayDataBuffer(g_buffer, sizeof(g_buffer));
}
