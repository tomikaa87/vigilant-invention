/*
 * lcd.h
 *
 *  Created on: 2018. dec. 28.
 *      Author: tomikaa
 */

#ifndef LCD_H_
#define LCD_H_


#include <stdint.h>

extern void LCD_WritePort(uint8_t data);
extern uint8_t LCD_ReadPort();
//extern void LCD_SetA0(uint8_t

enum {
    LCD_OFF,
    LCD_ON
};

enum {
    LCD_MX_0,
    LCD_MX_1,

    LCD_MY_0 = 1,
    LCD_MY_1
};

enum {
    LCD_FOSC_DIV_1,
    LCD_FOSC_DIV_2,
    LCD_FOSC_DIV_4,
    LCD_FOSC_DIV_8,
};

enum {
    LCD_NLINE_INV_FRAME
};

enum {
    LCD_BIAS_1_14,
    LCD_BIAS_1_13,
    LCD_BIAS_1_12,
    LCD_BIAS_1_11,
    LCD_BIAS_1_10,
    LCD_BIAS_1_9
};

enum {
    LCD_BOOSTER_X1,
    LCD_BOOSTER_X2,
    LCD_BOOSTER_X3,
    LCD_BOOSTER_X4,
    LCD_BOOSTER_X5,
    LCD_BOOSTER_X6,
    LCD_BOOSTER_X7,
    LCD_BOOSTER_X8
};

void LCD_Init();
void LCD_WriteCommand(uint8_t command);
void LCD_WriteData(uint8_t data);
uint8_t LCD_ReadData();
void LCD_Clear();
void LCD_DrawTestPattern();
void LCD_NOP();
void LCD_SoftReset();
void LCD_HardReset();
void LCD_SetPowerSave(uint8_t on);
void LCD_SetPartialMode(uint8_t on);
void LCD_SetInverseDisplay(uint8_t on);
void LCD_SetAllPixel(uint8_t on);
void LCD_SetDisplay(uint8_t on);
void LCD_SetColumnAddress(uint8_t start, uint8_t end);
void LCD_SetRowAddress(uint8_t start, uint8_t end);
void LCD_SetAddress(uint8_t column, uint8_t row);
void LCD_WriteDisplayData(uint8_t data);
void LCD_WriteDisplayDataBuffer(const uint8_t* const data, uint16_t length);
uint8_t LCD_ReadDisplayData();
void LCD_ReadDisplayDataBuffer(uint8_t* buffer, uint16_t length);
void LCD_SetPartialArea(uint8_t start, uint8_t end);
void LCD_SetScrollArea(uint8_t top, uint8_t scroll, uint8_t bottom);
void LCD_SetScanDirection(uint8_t mx, uint8_t my);
void LCD_SetDisplayStartLine(uint8_t start);
void LCD_SetMonochromeMode(uint8_t on);
void LCD_EnableDDRAMInterface();
void LCD_SetDisplayDuty(uint8_t value);
void LCD_SetFirstCOMOutput(uint8_t value);
void LCD_SetFOSCDivider(uint8_t value);
void LCD_EnablePartialDisplay();
void LCD_SetNLineInversion(uint8_t mode, uint8_t value);
void LCD_SetReadModifyWrite(uint8_t on);
void LCD_SetVop(uint16_t value);
void LCD_IncreaseVop();
void LCD_DecreaseVop();
void LCD_SetBiasSystem(uint8_t value);
void LCD_SetBoosterLevel(uint8_t value);
void LCD_SetVopOffset(uint8_t value);
void LCD_EnableAnalogControl();
void LCD_SetAutoReadControl(uint8_t on);

#endif /* LCD_H_ */
