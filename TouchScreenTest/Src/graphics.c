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

//    LCD_Clear();

//    // Top-left quadrand
//    LCD_SetRowAddress(0, y - 1);
//    LCD_SetColumnAddress(8, 8 + x / 2 - 1);
//    LCD_WriteCommand(0b00101100);
//    for (uint16_t i = 0; i < x * (y / 2); ++i)
//        LCD_WriteData(0xff);
//
//    // Bottom-left quadrand
//    LCD_SetRowAddress(y + 2, 159);
//    LCD_SetColumnAddress(8, 8 + x / 2 - 1);
//    LCD_WriteCommand(0b00101100);
//    for (uint16_t i = 0; i < x * (160 - y) / 2; ++i)
//        LCD_WriteData(0xff);
//
//    // Top-right quadrand
//    LCD_SetRowAddress(0, y - 1);
//    LCD_SetColumnAddress(8 + x / 2 + 1, 127);
//    LCD_WriteCommand(0b00101100);
//    for (uint16_t i = 0; i < (240 - x) * (y / 2); ++i)
//        LCD_WriteData(0xff);
//
//    // Bottom-right quadrand
//    LCD_SetRowAddress(y + 2, 159);
//    LCD_SetColumnAddress(8 + x / 2 + 1, 127);
//    LCD_WriteCommand(0b00101100);
//    for (uint16_t i = 0; i < (240 - x) * (160 - y) / 2; ++i)
//        LCD_WriteData(0xff);

    // Draw crosshair
//    LCD_SetRowAddress(0, 159);
//    LCD_SetColumnAddress(8 + x / 2 - 1, 8 + x / 2 - 1);
//
//    LCD_WriteCommand(0b00101100);
//    for (uint8_t i = 0; i < 160; ++i)
//        LCD_WriteData(0b11100000);
//
//    LCD_SetRowAddress(y, y);
//    LCD_SetColumnAddress(8, 127);
//
//    LCD_WriteCommand(0b00101100);
//    for (uint8_t i = 0; i < 240 / 2; ++i)
//        LCD_WriteData(0xff);

    LCD_SetAddress(x / 2, y);
    LCD_WriteDisplayData(0xff);
}

void Graphics_UpdateScreen()
{
    LCD_WriteDisplayDataBuffer(g_buffer, sizeof(g_buffer));
}
