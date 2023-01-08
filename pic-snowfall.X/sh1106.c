#include "sh1106.h"

#include <xc.h>

static bool _poweredOn = false;

enum {
    SH1106_I2C_ADDRESS = 0x3Cu,
    SH1106_I2C_DC_FLAG = 0x40u,
    SH1106_I2C_CO_FLAG = 0x80u
};

enum {
    SH1106_LCDWIDTH = 128u,
    SH1106_LCDHEIGHT = 64u,
    SH1106_LCDOFFSET = 2u,
    SH1106_PAGE_COUNT = 8u
};

enum {
    SH1106_CMD_SET_COL_ADDR_LOW = 0x00u,
    SH1106_CMD_SET_COL_ADDR_HIGH = 0x10u,
    SH1106_CMD_SET_CHARGE_PUMP_VOLTAGE = 0x30u,
    SH1106_CMD_SET_DISPLAY_START_LINE = 0x40u,
    SH1106_CMD_SET_CONTRAST_CONTROL = 0x81u,
    SH1106_CMD_SET_SEGMENT_REMAP = 0xA0u,
    SH1106_CMD_SET_ENTIRE_DISPLAY_OFF_ON = 0xA4u,
    SH1106_CMD_SET_NORMAL_REVERSE_DISPLAY = 0xA6u,
    SH1106_CMD_SET_MULTIPLEX_RATIO = 0xA8u,
    SH1106_CMD_SET_DC_DC_OFF_ON = 0xADu,
    SH1106_CMD_SET_DISPLAY_OFF_ON = 0xAEu,
    SH1106_CMD_SET_PAGE_ADDR = 0xB0u,
    SH1106_CMD_SET_COM_OUT_SCAN_DIR = 0xC0u,
    SH1106_CMD_SET_DISPLAY_OFFSET = 0xD3u,
    SH1106_CMD_SET_DISPLAY_CLOCK_DIV_RATIO = 0xD5u,
    SH1106_CMD_SET_PRECHARGE_PERIOD = 0xD9u,
    SH1106_CMD_SET_COM_PADS_HW_CONFIG = 0xDAu,
    SH1106_CMD_SET_VCOM_DESELECT_LEVEL = 0xDBu,
    SH1106_CMD_READ_MODIFY_WRITE = 0xE0u,
    SH1106_CMD_NOP = 0xE3u,
    SH1106_CMD_END_READ_MODIFY_WRITE = 0xEEu
};

static inline void i2cWait()
{
    while (
        SSP1STATbits.R_nW
        || (SSP1CON2 & 0b11111) // ACKEN | RCEN | PEN | RSEN | SEN
    );
}

static void i2cTransmit(const uint8_t* data, uint8_t length)
{
	// Start
	i2cWait();
	SSP1CON2bits.SEN = 1;

	// Send address
	i2cWait();
	SSP1BUF = SH1106_I2C_ADDRESS << 1;

	// Send data
	while (length-- > 0) {
		i2cWait();
		SSP1BUF = *data++;
	}

	// Stop
	i2cWait();
	SSP1CON2bits.PEN = 1;
}

void SH1106_init()
{
    SH1106_setChargePumpVoltage(SH1106_CPV_8_0V);
    SH1106_setDisplayStartLine(0);
    SH1106_setContrast(0x80);
    SH1106_setSegmentRemap(true);
    SH1106_setInvertedDisplay(false);
    SH1106_setMultiplexRatio(63);
    SH1106_setDcDcConvOn(true);
    SH1106_setComScanInverted(true);
    SH1106_setDisplayOffset(0);
    SH1106_setDisplayClockDivRatio(0, 0x0F);
    SH1106_setDischargePrechargePeriod(2, 2);
    SH1106_setComPadsAltHwConfig(true);
    SH1106_setVcomDeselectLevel(0x35);
}

void SH1106_fill(const uint8_t pattern)
{
    SH1106_setColumn(0);

    for (uint8_t line = 0; line < SH1106_PAGES; ++line)
    {
        SH1106_setLine(line);

        for (uint8_t i = 0; i < 132; ++i)
            SH1106_sendData(pattern, 0, false);
    }
}

void SH1106_sendCommand(uint8_t code)
{
    uint8_t data[] = {
        SH1106_I2C_CO_FLAG,
        code
    };

    i2cTransmit(data, sizeof(data));
}

void SH1106_sendCommandWithArg(uint8_t code, uint8_t arg)
{
    uint8_t data[] = {
        SH1106_I2C_CO_FLAG,
        code,
        SH1106_I2C_CO_FLAG,
        arg
    };

    i2cTransmit(data, sizeof(data));
}

void SH1106_sendData(uint8_t data, const uint8_t bitShift, const bool invert)
{
    if (bitShift > 0)
        data <<= bitShift;

    if (invert)
        data = ~data;

    uint8_t buf[] = {
        SH1106_I2C_DC_FLAG,
        data
    };

    i2cTransmit(buf, sizeof(buf));
}

void SH1106_sendDataArray(const uint8_t* data, const uint16_t length, const uint8_t bitShift, const bool invert)
{
    if (bitShift > 7)
        return;

    uint8_t buffer[17];
    uint16_t bytesRemaining = length;
    uint8_t dataIndex = 0;
    static const uint8_t chunk_size = sizeof(buffer) - 1;

    while (bytesRemaining > 0) {
        uint8_t count = bytesRemaining >= chunk_size ? chunk_size : (uint8_t)bytesRemaining;
        bytesRemaining -= count;

        buffer[0] = SH1106_I2C_DC_FLAG;

        for (uint8_t i = 1; i <= count; ++i) {
            uint8_t byte = (uint8_t)(data[dataIndex++] << bitShift);
            if (invert) {
                byte = ~byte;
            }
            buffer[i] = byte;
        }

        i2cTransmit(buffer, count + 1);
    }
}

bool SH1106_isPoweredOn()
{
    return _poweredOn;
}

void SH1106_setPowerOn(const bool on)
{
    _poweredOn = on;
    SH1106_sendCommand(SH1106_CMD_SET_DISPLAY_OFF_ON | (on ? 1u : 0u));
}

void SH1106_setChargePumpVoltage(const SH1106_ChargePumpVoltage voltage)
{
    SH1106_sendCommand(SH1106_CMD_SET_CHARGE_PUMP_VOLTAGE | ((uint8_t)voltage & 0x03u));
}

void SH1106_setComPadsAltHwConfig(const bool alternative)
{
    SH1106_sendCommandWithArg(
        SH1106_CMD_SET_COM_PADS_HW_CONFIG,
        alternative ? 0x12u : 0x02u
    );
}

void SH1106_setComScanInverted(const bool inverted)
{
    SH1106_sendCommand(SH1106_CMD_SET_COM_OUT_SCAN_DIR | (inverted ? 0x08u : 0u));
}

void SH1106_setContrast(const uint8_t value)
{
    SH1106_sendCommandWithArg(SH1106_CMD_SET_CONTRAST_CONTROL, value);
}

void SH1106_setDcDcConvOn(const bool on)
{
    SH1106_sendCommandWithArg(SH1106_CMD_SET_DC_DC_OFF_ON, 0x8Au | (on ? 1u : 0u));
}

void SH1106_setDischargePrechargePeriod(const uint8_t precharge, const uint8_t discharge)
{
    SH1106_sendCommandWithArg(
        SH1106_CMD_SET_PRECHARGE_PERIOD,
        (precharge & 0x0Fu) | ((uint8_t)(discharge << 4) & 0x0Fu)
    );
}

void SH1106_setDisplayClockDivRatio(const uint8_t div, const uint8_t ratio)
{
    SH1106_sendCommandWithArg(
        SH1106_CMD_SET_DISPLAY_CLOCK_DIV_RATIO,
        (div & 0x0Fu) | ((uint8_t)(ratio << 4) & 0xF0u)
    );
}

void SH1106_setDisplayStartLine(const uint8_t value)
{
    SH1106_sendCommand(SH1106_CMD_SET_DISPLAY_START_LINE | (value & 0x3Fu));
}

void SH1106_setDisplayOffset(const uint8_t value)
{
    SH1106_sendCommandWithArg(SH1106_CMD_SET_DISPLAY_OFFSET, value & 0x3Fu);
}

void SH1106_setInvertedDisplay(const bool inverted)
{
    SH1106_sendCommand(SH1106_CMD_SET_NORMAL_REVERSE_DISPLAY | (inverted ? 1u : 0u));
}

void SH1106_setMultiplexRatio(const uint8_t value)
{
    SH1106_sendCommandWithArg(SH1106_CMD_SET_MULTIPLEX_RATIO, value & 0x3Fu);
}

void SH1106_setSegmentRemap(const bool reverse)
{
    SH1106_sendCommand(SH1106_CMD_SET_SEGMENT_REMAP | (reverse ? 1u : 0u));
}

void SH1106_setVcomDeselectLevel(const uint8_t value)
{
    SH1106_sendCommandWithArg(SH1106_CMD_SET_VCOM_DESELECT_LEVEL, value);
}

void SH1106_setColumn(const uint8_t column)
{
    uint8_t addr = column + SH1106_LCDOFFSET;

    SH1106_sendCommandWithArg(
        SH1106_CMD_SET_COL_ADDR_LOW | (addr & 0x0Fu),
        SH1106_CMD_SET_COL_ADDR_HIGH | ((addr >> 4) & 0x0Fu)
    );
}

void SH1106_setLine(const uint8_t line)
{
    SH1106_sendCommand(SH1106_CMD_SET_PAGE_ADDR | (line & 0x0F));
}
