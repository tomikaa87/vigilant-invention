/*
 * lcd.c
 *
 *  Created on: 2018. dec. 28.
 *      Author: tomikaa
 */


#include "lcd.h"
#include "lcdconfig.h"
#include "main.h"

#include <stdint.h>


void LCD_Init()
{
    LCD_HardReset();
    LCD_SetPowerSave(LCD_OFF);
    LCD_SetPartialMode(LCD_OFF);
    LCD_SetDisplay(LCD_OFF);

    LCD_DELAY_MS(50);

    LCD_SetVop(0x130);
    LCD_SetBiasSystem(LCD_BIAS_1_14);
    LCD_SetBoosterLevel(LCD_BOOSTER_X8);
    LCD_EnableAnalogControl();
    LCD_SetNLineInversion(0, 0);
    LCD_SetMonochromeMode(LCD_OFF);
    LCD_EnableDDRAMInterface();
    LCD_SetScanDirection(LCD_ON, LCD_ON);
    LCD_SetDisplayDuty(0x9f);
    LCD_SetInverseDisplay(LCD_OFF);

    LCD_Clear();

    LCD_SetDisplay(LCD_ON);
}

void LCD_WriteCommand(uint8_t command)
{
    LCD_RESET_A0();
    LCD_RESET_CSB();
    LCD_RESET_RW();

    LCD_WRITE_PORT(command);

    LCD_SET_CSB();
}

void LCD_WriteData(uint8_t data)
{
    LCD_SET_A0();
    LCD_RESET_CSB();
    LCD_RESET_RW();

    LCD_WRITE_PORT(data);

    LCD_SET_CSB();
}

uint8_t LCD_ReadData()
{
    LCD_SET_A0();
    LCD_RESET_CSB();
    LCD_SET_RW();

    uint8_t data = LCD_READ_PORT();

    LCD_SET_CSB();

    return data;
}

void LCD_Clear()
{
    LCD_SetAddress(0, 0);
    LCD_WriteCommand(0x2c);
    for (uint16_t i = 0; i < 160 * 120; ++i)
        LCD_WriteData(0x00);
}

void LCD_DrawTestPattern()
{
    LCD_SetAddress(0, 0);
    LCD_WriteCommand(0x2c);

    LCD_SET_A0();
    LCD_RESET_RW();

    for (uint16_t i = 0; i < 120 * 40; ++i) {
        LCD_WRITE_PORT(0xff);
        LCD_RESET_CSB();
        LCD_SET_CSB();
    }

    for (uint16_t i = 0; i < 120 * 40; ++i) {
        LCD_WRITE_PORT(0b10010010);
        LCD_RESET_CSB();
        LCD_SET_CSB();
    }

    for (uint16_t i = 0; i < 120 * 40; ++i) {
        LCD_WRITE_PORT(0b01001001);
        LCD_RESET_CSB();
        LCD_SET_CSB();
    }

    for (uint16_t i = 0; i < 120 * 40; ++i) {
        LCD_WRITE_PORT(0);
        LCD_RESET_CSB();
        LCD_SET_CSB();
    }
}

void LCD_NOP()
{
    LCD_WriteCommand(0);
}

void LCD_SoftReset()
{
    LCD_WriteCommand(0b00000001);
}

void LCD_HardReset()
{
    LCD_DELAY_MS(5);
    LCD_SET_RST();
    LCD_DELAY_MS(5);
    LCD_RESET_RST();
    LCD_DELAY_MS(10);
    LCD_SET_RST();
    LCD_DELAY_MS(50);

    LCD_SET_E();
}

void LCD_SetPowerSave(uint8_t on)
{
    LCD_WriteCommand(0b00010000 | (on ? 0 : 1));
}

void LCD_SetPartialMode(uint8_t on)
{
    LCD_WriteCommand(0b00010010 | (on & 1));
}

void LCD_SetInverseDisplay(uint8_t on)
{
    LCD_WriteCommand(0b00100000 | (on & 1));
}

void LCD_SetAllPixel(uint8_t on)
{
    LCD_WriteCommand(0b00100010 | (on & 1));
}

void LCD_SetDisplay(uint8_t on)
{
    LCD_WriteCommand(0b00101000 | (on & 1));
}

void LCD_SetColumnAddress(uint8_t start, uint8_t end)
{
    LCD_WriteCommand(0b00101010);
    LCD_WriteData(0);
    LCD_WriteData(start & 0xff);
    LCD_WriteData(0);
    LCD_WriteData(end & 0xff);
}

void LCD_SetRowAddress(uint8_t start, uint8_t end)
{
    LCD_WriteCommand(0b00101011);
    LCD_WriteData(0);
    LCD_WriteData(start & 0xff);
    LCD_WriteData(0);
    LCD_WriteData(end & 0xff);
}

void LCD_SetAddress(uint8_t column, uint8_t row)
{
    LCD_SetColumnAddress(column + 8, 0x7f);
    LCD_SetRowAddress(row, 0x9f);
}

void LCD_WriteDisplayData(uint8_t data)
{
    LCD_WriteCommand(0b00101100);
    LCD_WriteData(data);
}

void LCD_WriteDisplayDataBuffer(const uint8_t* const data, uint16_t length)
{
    LCD_WriteCommand(0b00101100);

    LCD_SET_A0();
    LCD_RESET_RW();

    for (uint16_t i = 0; i < length; ++i) {
        LCD_WRITE_PORT(data[i]);
        LCD_RESET_CSB();
        LCD_SET_CSB();
    }
}

uint8_t LCD_ReadDisplayData()
{
    LCD_WriteCommand(0b00101110);
    return LCD_ReadData();
}

void LCD_ReadDisplayDataBuffer(uint8_t* buffer, uint16_t length)
{
    LCD_WriteCommand(0b00101110);
    for (uint16_t i = 0; i < length; ++i)
        buffer[i] = LCD_ReadData();
}

void LCD_SetPartialArea(uint8_t start, uint8_t end)
{
    LCD_WriteCommand(0b00110000);
    LCD_WriteData(0);
    LCD_WriteData(start);
    LCD_WriteData(0);
    LCD_WriteData(end);
}

void LCD_SetScrollArea(uint8_t top, uint8_t scroll, uint8_t bottom)
{
    LCD_WriteCommand(0b00110011);
    LCD_WriteData(top);
    LCD_WriteData(scroll);
    LCD_WriteData(bottom);
}

void LCD_SetScanDirection(uint8_t mx, uint8_t my)
{
    LCD_WriteCommand(0b00110110);
    LCD_WriteData((mx ? 0b10000000 : 0) | (my ? 0b1000000 : 0));
}

void LCD_SetDisplayStartLine(uint8_t start)
{
    LCD_WriteCommand(0b00110111);
    LCD_WriteData(start);
}

void LCD_SetMonochromeMode(uint8_t on)
{
    LCD_WriteCommand(0b00111000 | (on & 1));
}

void LCD_EnableDDRAMInterface()
{
    LCD_WriteCommand(0b00111010);
    LCD_WriteData(0b00000010);
}

void LCD_SetDisplayDuty(uint8_t value)
{
    if (value < 0x03 || value > 0x9f)
        return;

    LCD_WriteCommand(0b10110000);
    LCD_WriteData(value);
}

void LCD_SetFirstCOMOutput(uint8_t value)
{
    if (value > 0x9f)
        return;

    LCD_WriteCommand(0b10110001);
    LCD_WriteData(value);
}

void LCD_SetFOSCDivider(uint8_t value)
{
    if (value & 0b11111100)
        return;

    LCD_WriteCommand(0b10110011);
    LCD_WriteData(value);
}

void LCD_EnablePartialDisplay()
{
    LCD_WriteCommand(0b10110100);
    LCD_WriteData(0b10100000);
}

void LCD_SetNLineInversion(uint8_t mode, uint8_t value)
{
    LCD_WriteCommand(0b10110101);
    LCD_WriteData((mode ? 0b10000000 : 0) | (value & 0b11111));
}

void LCD_SetReadModifyWrite(uint8_t on)
{
    LCD_WriteCommand(0b10111000 | (on ? 1 : 0));
}

void LCD_SetVop(uint16_t value)
{
    LCD_WriteCommand(0b11000000);
    LCD_WriteData(value & 0xff);
    LCD_WriteData(value >> 8);
}

void LCD_IncreaseVop()
{
    LCD_WriteCommand(0b11000001);
}

void LCD_DecreaseVop()
{
    LCD_WriteCommand(0b11000010);
}

void LCD_SetBiasSystem(uint8_t value)
{
    LCD_WriteCommand(0b11000011);
    LCD_WriteData(value & 0b111);
}

void LCD_SetBoosterLevel(uint8_t value)
{
    LCD_WriteCommand(0b11000100);
    LCD_WriteData(value & 0b111);
}

void LCD_SetVopOffset(uint8_t value)
{
    LCD_WriteCommand(0b11000111);
    LCD_WriteData(value & 0b1111111);
}

void LCD_EnableAnalogControl()
{
    LCD_WriteCommand(0b11010000);
    LCD_WriteData(0b00011101);
}

void LCD_SetAutoReadControl(uint8_t on)
{
    LCD_WriteCommand(0b11010111);
    LCD_WriteData(0b10001111 | (on ? 0b10000 : 0));
}
