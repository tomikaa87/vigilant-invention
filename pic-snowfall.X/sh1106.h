/*
 * File:   sh1106.h
 * Author: tomikaa
 *
 * Created on January 3, 2023, 9:07 PM
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#define SH1106_WIDTH        128u
#define SH1106_HEIGHT       64u
#define SH1106_PAGES        8u

typedef enum
{
    SH1106_CPV_6_4V,
    SH1106_CPV_7_4V,
    SH1106_CPV_8_0V,
    SH1106_CPV_9_0V
} SH1106_ChargePumpVoltage;

void SH1106_init(void);
void SH1106_fill(uint8_t pattern);
void SH1106_sendCommand(uint8_t code);
void SH1106_sendCommandWithArg(uint8_t code, uint8_t arg);
void SH1106_sendData(uint8_t data, uint8_t bitShift, bool invert);
void SH1106_sendDataArray(const uint8_t* data, uint16_t length, uint8_t bitShift, bool invert);
bool SH1106_isPoweredOn(void);
void SH1106_setPowerOn(bool on);
void SH1106_setChargePumpVoltage(SH1106_ChargePumpVoltage voltage);
void SH1106_setComPadsAltHwConfig(bool alternative);
void SH1106_setComScanInverted(bool inverted);
void SH1106_setContrast(uint8_t value);
void SH1106_setDcDcConvOn(bool on);
void SH1106_setDischargePrechargePeriod(uint8_t precharge, uint8_t discharge);
void SH1106_setDisplayClockDivRatio(uint8_t div, uint8_t ratio);
void SH1106_setDisplayStartLine(uint8_t value);
void SH1106_setDisplayOffset(uint8_t value);
void SH1106_setInvertedDisplay(bool inverted);
void SH1106_setMultiplexRatio(uint8_t value);
void SH1106_setSegmentRemap(bool reverse);
void SH1106_setVcomDeselectLevel(uint8_t value);
void SH1106_setColumn(uint8_t column);
void SH1106_setLine(uint8_t line);