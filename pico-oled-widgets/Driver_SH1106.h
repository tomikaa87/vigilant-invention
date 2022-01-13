/*
    This file is part of esp-thermostat.

    esp-thermostat is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    esp-thermostat is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with esp-thermostat.  If not, see <http://www.gnu.org/licenses/>.

    Author: Tamas Karpati
    Created on 2020-01-22
*/

#pragma once

#include <cstdint>
#include <cstdlib>

namespace Driver
{

template <typename I2cAdapter>
class SH1106
{
public:
    enum class ChargePumpVoltage
    {
        _6_4V,
        _7_4V,
        _8_0V,
        _9_0V
    };

    static constexpr auto Height = 64u;
    static constexpr auto Width = 128u;
    static constexpr auto Lines = 8u;

    static void init()
    {
        setChargePumpVoltage(ChargePumpVoltage::_8_0V);
        setDisplayStartLine(0);
        setContrast(0x80);
        setSegmentRemap(true);
        setInvertedDisplay(false);
        setMultiplexRatio(63);
        setDcDcConvOn(true);
        setComScanInverted(true);
        setDisplayOffset(0);
        setDisplayClockDivRatio(0, 0x0F);
        setDischargePrechargePeriod(2, 2);
        setComPadsAltHwConfig(true);
        setVcomDeselectLevel(0x35);
    }

    static void fill(const uint8_t pattern)
    {
        setColumn(0);

        for (uint8_t line = 0; line < Lines; ++line)
        {
            setLine(line);

            for (uint8_t i = 0; i < 132; ++i)
                sendData(pattern);
        }
    }

    static void sendCommand(const uint8_t code)
    {
        const uint8_t data[] = {
            SH1106_I2C_CO_FLAG,
            code
        };

        I2cAdapter::writeData(SH1106_I2C_ADDRESS, data, sizeof(data));
    }

    static void sendCommand(const uint8_t code, const uint8_t arg)
    {
        const uint8_t data[] = {
            SH1106_I2C_CO_FLAG,
            code,
            SH1106_I2C_CO_FLAG,
            arg
        };

        I2cAdapter::writeData(SH1106_I2C_ADDRESS, data, sizeof(data));
    }

    static void sendData(const uint8_t data, const uint8_t bitShift = 0, const bool invert = false)
    {
        sendData(&data, sizeof(data), bitShift, invert);
    }

    static void sendData(const uint8_t* const data, const size_t length, const uint8_t bitShift = 0, const bool invert = false)
    {
        if (bitShift > 7)
            return;

        I2cAdapter::writeData(
            SH1106_I2C_ADDRESS, 
            SH1106_I2C_DC_FLAG,
            data, length,
            [bitShift, invert](uint8_t byte) {
                if (bitShift > 0)
                    byte <<= bitShift;

                if (invert)
                    byte = ~byte;

                return byte;
            }
        );
    }

    static void setPowerOn(const bool on)
    {
        sendCommand(SH1106_CMD_SET_DISPLAY_OFF_ON | (on ? 1u : 0u));
    }

    static void setChargePumpVoltage(const ChargePumpVoltage voltage)
    {
        sendCommand(SH1106_CMD_SET_CHARGE_PUMP_VOLTAGE | (static_cast<uint8_t>(voltage) & 0x03u));
    }

    static void setComPadsAltHwConfig(const bool alternative)
    {
        sendCommand(
            SH1106_CMD_SET_COM_PADS_HW_CONFIG,
            alternative ? 0x12u : 0x02u
        );
    }

    static void setComScanInverted(const bool inverted)
    {
        sendCommand(SH1106_CMD_SET_COM_OUT_SCAN_DIR | (inverted ? 0x08u : 0u));
    }

    static void setContrast(const uint8_t value)
    {
        sendCommand(SH1106_CMD_SET_CONTRAST_CONTROL, value);
    }

    static void setDcDcConvOn(const bool on)
    {
        sendCommand(SH1106_CMD_SET_DC_DC_OFF_ON, 0x8Au | (on ? 1u : 0u));
    }

    static void setDischargePrechargePeriod(const uint8_t precharge, const uint8_t discharge)
    {
        sendCommand(
            SH1106_CMD_SET_PRECHARGE_PERIOD,
            (precharge & 0x0Fu) | (static_cast<uint8_t>(discharge << 4) & 0x0Fu)
        );
    }

    static void setDisplayClockDivRatio(const uint8_t div, const uint8_t ratio)
    {
        sendCommand(
            SH1106_CMD_SET_DISPLAY_CLOCK_DIV_RATIO,
            (div & 0x0Fu) | (static_cast<uint8_t>(ratio << 4) & 0xF0u)
        );
    }

    static void setDisplayStartLine(const uint8_t value)
    {
        sendCommand(SH1106_CMD_SET_DISPLAY_START_LINE | (value & 0x3Fu));
    }

    static void setDisplayOffset(const uint8_t value)
    {
        sendCommand(SH1106_CMD_SET_DISPLAY_OFFSET, value & 0x3Fu);
    }

    static void setInvertedDisplay(const bool inverted)
    {
        sendCommand(SH1106_CMD_SET_NORMAL_REVERSE_DISPLAY | (inverted ? 1u : 0u));
    }

    static void setMultiplexRatio(const uint8_t value)
    {
        sendCommand(SH1106_CMD_SET_MULTIPLEX_RATIO, value & 0x3Fu);
    }

    static void setSegmentRemap(const bool reverse)
    {
        sendCommand(SH1106_CMD_SET_SEGMENT_REMAP | (reverse ? 1u : 0u));
    }

    static void setVcomDeselectLevel(const uint8_t value)
    {
        sendCommand(SH1106_CMD_SET_VCOM_DESELECT_LEVEL, value);
    }

    static void setColumn(const uint8_t column)
    {
        const auto addr = column + SH1106_LCDOFFSET;

        sendCommand(
            SH1106_CMD_SET_COL_ADDR_LOW | (addr & 0x0Fu),
            SH1106_CMD_SET_COL_ADDR_HIGH | ((addr >> 4) & 0x0Fu)
        );
    }

    static void setLine(const uint8_t line)
    {
        sendCommand(SH1106_CMD_SET_PAGE_ADDR | (line & 0x0F));
    }

private:
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
};

}
