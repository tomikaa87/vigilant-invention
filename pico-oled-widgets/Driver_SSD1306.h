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

enum class SSD1306_Resolution
{
    R128x64_P8,
    R128x32_P4,
    R96x16_P2
};

template <
    typename I2cAdapter,
    SSD1306_Resolution Resolution
>
class SSD1306
{
public:
    enum class MemoryMode : uint8_t
    {
        HorizontalAddressing,
        VerticalAddressing,
        PageAddressing
    };

    static constexpr auto Width = [] {
        switch (Resolution) {
            case SSD1306_Resolution::R128x64_P8:
            case SSD1306_Resolution::R128x32_P4:
                return 128u;
            case SSD1306_Resolution::R96x16_P2:
                return 96u;
        }
        return 0u;
    }();

    static constexpr auto Height = [] {
        switch (Resolution) {
            case SSD1306_Resolution::R128x64_P8:
                return 64u;
            case SSD1306_Resolution::R128x32_P4:
                return 32u;
            case SSD1306_Resolution::R96x16_P2:
                return 16u;
        }
        return 0u;
    }();

    static constexpr auto Lines = [] {
        switch (Resolution) {
            case SSD1306_Resolution::R128x64_P8:
                return 8u;
            case SSD1306_Resolution::R128x32_P4:
                return 4u;
            case SSD1306_Resolution::R96x16_P2:
                return 2u;
        }
        return 0u;
    }();

    static void init()
    {
        setDisplayClockDivRatio(0, 8);
        setMultiplexRatio(Height - 1);
        setDisplayOffset(0);
        setDisplayStartLine(0);
        setDcDcConvOn(true);
        setMemoryMode(MemoryMode::PageAddressing);
        setSegmentRemap(true);
        setComScanInverted(true);
        setComPadsAltHwConfig(0x12);
        setContrast(0x60);
        setVcomDeselectLevel(0x10);
        setInvertedDisplay(false);
        setDischargePrechargePeriod(2, 2);
    }

    static void fill(const uint8_t pattern)
    {
        setColumn(0);

        for (uint8_t line = 0; line < Lines; ++line)
        {
            setLine(line);

            for (uint8_t i = 0; i < Width; ++i)
                sendData(pattern);
        }
    }

    static void sendCommand(const uint8_t code)
    {
        const uint8_t data[] = {
            SSD1306_I2C_CO_FLAG,
            code
        };

        I2cAdapter::writeData(SSD1306_I2C_ADDRESS, data, sizeof(data));
    }

    static void sendCommand(const uint8_t code, const uint8_t arg)
    {
        const uint8_t data[] = {
            SSD1306_I2C_CO_FLAG,
            code,
            SSD1306_I2C_CO_FLAG,
            arg
        };

        I2cAdapter::writeData(SSD1306_I2C_ADDRESS, data, sizeof(data));
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
            SSD1306_I2C_ADDRESS, 
            SSD1306_I2C_DC_FLAG,
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
    
    static void setPowerOn(bool on)
    {
        sendCommand(SSD1306_CMD_DISPLAYOFF | (on ? 1u : 0u));
    }

    static void setComPadsAltHwConfig(const uint8_t value)
    {
        sendCommand(SSD1306_CMD_SETCOMPINS, value & 0b00110010);
    }

    static void setComScanInverted(const bool inverted)
    {
        sendCommand(SSD1306_CMD_COMSCANINC | (inverted ? 0x08u : 0u));
    }

    static void setContrast(const uint8_t value)
    {
        sendCommand(SSD1306_CMD_SETCONTRAST, value);
    }

    static void setDcDcConvOn(const bool on)
    {
        sendCommand(SSD1306_CMD_CHARGEPUMP, on ? 0x14u : 0x10u);
    }

    static void setDischargePrechargePeriod(const uint8_t precharge, const uint8_t discharge)
    {
        sendCommand(
            SSD1306_CMD_SETPRECHARGE,
            (precharge & 0x0Fu) | (static_cast<uint8_t>(discharge << 4) & 0x0Fu)
        );
    }

    static void setDisplayAllOn(const bool on)
    {
        sendCommand(SSD1306_CMD_DISPLAYOFF | (on ? 1u : 0u));
    }

    static void setDisplayClockDivRatio(const uint8_t div, const uint8_t ratio)
    {
        sendCommand(
            SSD1306_CMD_SETDISPLAYCLOCKDIV,
            (div & 0x0Fu) | (static_cast<uint8_t>(ratio << 4) & 0xF0u)
        );
    }

    static void setDisplayStartLine(const uint8_t value)
    {
        sendCommand(SSD1306_CMD_SETSTARTLINE | (value & 0x3Fu));
    }

    static void setDisplayOffset(const uint8_t value)
    {
        sendCommand(SSD1306_CMD_SETDISPLAYOFFSET, value & 0x3Fu);
    }

    static void setInvertedDisplay(const bool inverted)
    {
        sendCommand(SSD1306_CMD_NORMALDISPLAY | (inverted ? 1u : 0u));
    }

    static void setMemoryMode(const MemoryMode mode)
    {
        sendCommand(SSD1306_CMD_MEMORYMODE, static_cast<uint8_t>(mode));
    }

    static void setMultiplexRatio(const uint8_t value)
    {
        sendCommand(SSD1306_CMD_SETMULTIPLEX, value);
    }

    static void setSegmentRemap(const bool reverse)
    {
        sendCommand(SSD1306_CMD_SEGREMAP | (reverse ? 1u : 0u));
    }

    static void setVcomDeselectLevel(const uint8_t value)
    {
        sendCommand(SSD1306_CMD_SETVCOMDETECT, value & 0b01110000);
    }

    static void setColumn(const uint8_t column)
    {
        sendCommand(
            SSD1306_CMD_SETLOWCOLUMN | (column & 0x0Fu),
            SSD1306_CMD_SETHIGHCOLUMN | ((column >> 4) & 0x0Fu)
        );
    }

    static void setLine(const uint8_t line)
    {
        sendCommand(SSD1306_CMD_PAGESTARTADDR | (line & 0x0F));
    }

private:
    enum {
        SSD1306_I2C_ADDRESS = 0x3Cu,
        SSD1306_I2C_DC_FLAG = 0x40u,
        SSD1306_I2C_CO_FLAG = 0x80u
    };

    enum {
        SSD1306_CMD_SETCONTRAST = 0x81u,
        SSD1306_CMD_DISPLAYALLON_RESUME = 0xA4u,
        SSD1306_CMD_DISPLAYALLON = 0xA5u,
        SSD1306_CMD_NORMALDISPLAY = 0xA6u,
        SSD1306_CMD_INVERTDISPLAY = 0xA7u,
        SSD1306_CMD_DISPLAYOFF = 0xAEu,
        SSD1306_CMD_DISPLAYON = 0xAFu,
        SSD1306_CMD_SETDISPLAYOFFSET = 0xD3u,
        SSD1306_CMD_SETCOMPINS = 0xDAu,
        SSD1306_CMD_SETVCOMDETECT = 0xDBu,
        SSD1306_CMD_SETDISPLAYCLOCKDIV = 0xD5u,
        SSD1306_CMD_SETPRECHARGE = 0xD9u,
        SSD1306_CMD_SETMULTIPLEX = 0xA8u,
        SSD1306_CMD_SETLOWCOLUMN = 0x00u,
        SSD1306_CMD_SETHIGHCOLUMN = 0x10u,
        SSD1306_CMD_SETSTARTLINE = 0x40u,
        SSD1306_CMD_MEMORYMODE = 0x20u,
        SSD1306_CMD_COLUMNADDR = 0x21u,
        SSD1306_CMD_PAGEADDR = 0x22u,
        SSD1306_CMD_COMSCANINC = 0xC0u,
        SSD1306_CMD_COMSCANDEC = 0xC8u,
        SSD1306_CMD_SEGREMAP = 0xA0u,
        SSD1306_CMD_CHARGEPUMP = 0x8Du,
        SSD1306_CMD_EXTERNALVCC = 0x01u,
        SSD1306_CMD_SWITCHCAPVCC = 0x02u,
        SSD1306_CMD_ACTIVATE_SCROLL = 0x2Fu,
        SSD1306_CMD_DEACTIVATE_SCROLL = 0x2Eu,
        SSD1306_CMD_SET_VERTICAL_SCROLL_AREA = 0xA3u,
        SSD1306_CMD_RIGHT_HORIZONTAL_SCROLL = 0x26u,
        SSD1306_CMD_LEFT_HORIZONTAL_SCROLL = 0x27u,
        SSD1306_CMD_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 0x29u,
        SSD1306_CMD_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL = 0x2Au,
        SSD1306_CMD_PAGESTARTADDR = 0xB0u,
    };

    enum {
        SSD1306_MEM_MODE_PAGE_ADDRESSING = 0b10,
        SSD1306_MEM_MODE_HORIZONTAL_ADDRESSING = 0,
        SSD1306_MEM_MODE_VERTICAL_ADDRESSING = 0b01
    };

    enum {
        SSD1306_SCROLL_STOP,
        SSD1306_SCROLL_LEFT,
        SSD1306_SCROLL_RIGHT,
        SSD1306_SCROLL_DIAG_LEFT,
        SSD1306_SCROLL_DIAG_RIGHT
    };
};

}
