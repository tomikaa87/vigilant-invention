/*
 * graphics.c
 *
 *  Created on: 2018. dec. 30.
 *      Author: tomikaa
 */

#include "graphics.h"
#include "lcd.h"

#include <stdbool.h>

static uint8_t g_buffer[240 / 3 * 160] = { 0, };

void drawPalette()
{
    LCD_SetColumnAddress(18, 27);
    LCD_SetRowAddress(0, 19);
    LCD_WriteCommand(0x2c);
    for (uint16_t i = 0; i < 200; ++i)
        LCD_WriteData(0b11011000);

    LCD_SetColumnAddress(28, 37);
    LCD_SetRowAddress(0, 19);
    LCD_WriteCommand(0x2c);
    for (uint16_t i = 0; i < 200; ++i)
        LCD_WriteData(0b10010000);

    LCD_SetColumnAddress(38, 47);
    LCD_SetRowAddress(0, 19);
    LCD_WriteCommand(0x2c);
    for (uint16_t i = 0; i < 200; ++i)
        LCD_WriteData(0b01001000);
}

bool isInRect(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    return x > x1 && x < x2 && y > y1 && y < y2;
}

void Graphics_DrawCursor(uint8_t x, uint8_t y)
{
    static uint8_t lastX = 0;
    static uint8_t lastY = 0;
    static uint8_t color = GRAPHICS_COLOR_BLACK;

    if (lastX == x && lastY == y)
        return;

    lastX = x;
    lastY = y;

    if (x < 10 && y < 10)
    {
        LCD_Clear();
        drawPalette();
    }
    else if (isInRect(x, y, 20, 0, 40, 20))
    {
        color = GRAPHICS_COLOR_BLACK;
    }
    else if (isInRect(x, y, 40, 0, 60, 20))
    {
        color = GRAPHICS_COLOR_DARKGRAY;
    }
    else if (isInRect(x, y, 60, 0, 80, 20))
    {
        color = GRAPHICS_COLOR_LIGHTGRAY;
    }
    else if (isInRect(x, y, 80, 0, 100, 20))
    {
        color = GRAPHICS_COLOR_WHITE;
    }
    else
    {
        Graphics_SetPixel(x, y, color);
    }
}

void Graphics_UpdateScreen()
{
    LCD_WriteDisplayDataBuffer(g_buffer, sizeof(g_buffer));
}

void Graphics_SetPixel(uint8_t x, uint8_t y, uint8_t color)
{
    LCD_SetAddress(x / 2, y);
    LCD_SetReadModifyWrite(LCD_ON);
    uint8_t d = LCD_ReadData();

    color &= 0b11;

    if (!(x & 1))
    {
        d &= 0b11100011;
        d |= color << 3;
    }
    else
    {
        d &= 0b00011111;
        d |= color << 6;
    }

    LCD_WriteData(d);
    LCD_SetReadModifyWrite(LCD_OFF);
}

void Graphics_DrawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
    for (uint8_t x = x1; x <= x2; ++x)
    {
        Graphics_SetPixel(x, y1, color);
        Graphics_SetPixel(x, y2, color);
    }

    for (uint8_t y = y1 + 1; y <= y2 - 1; ++y)
    {
        Graphics_SetPixel(x1, y, color);
        Graphics_SetPixel(x2, y, color);
    }
}

void Graphics_FillRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
    for (uint8_t x = x1; x <= x2; ++x)
        for (uint8_t y = y1; y <= y2; ++y)
            Graphics_SetPixel(x, y, color);
}
