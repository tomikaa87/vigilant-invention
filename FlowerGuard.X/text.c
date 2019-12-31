/*
    This file is part of pic-thermostat.

    pic-thermostat is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    pic-thermostat is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with pic-thermostat.  If not, see <http://www.gnu.org/licenses/>.

    Author: Tamas Karpati
    Created on 2017-01-01
*/

#include "text.h"
#include "ssd1306.h"

#include <string.h>

static const uint8_t normal_charset[59][TEXT_NORMAL_CHAR_WIDTH] = {
	// [space]
	{
		0, 0, 0, 0, 0
	},
	// !
	{
		0b00000000,
		0b00000000,
		0b01001111,
		0b00000000,
		0b00000000
	},
	// "
	{
		0b00000000,
		0b00000111,
		0b00000000,
		0b00000111,
		0b00000000
	},
	// #
	{
		0b00010100,
		0b01111111,
		0b00010100,
		0b01111111,
		0b00010100
	},
	// $
	{
		0b00100100,
		0b00101010,
		0b01111111,
		0b00101010,
		0b00010010
	},
	// %
	{
		0b00100011,
		0b00010011,
		0b00001000,
		0b01100100,
		0b01100010
	},
	// &
	{
		0b00110110,
		0b01001001,
		0b01010101,
		0b00100010,
		0b01010000
	},
	// '
	{
		0b00000000,
		0b00000101,
		0b00000011,
		0b00000000,
		0b00000000
	},
	// (
	{
		0b00000000,
		0b00011100,
		0b00100010,
		0b01000001,
		0b00000000
	},
	// )
	{
		0b00000000,
		0b01000001,
		0b00100010,
		0b00011100,
		0b00000000
	},
	// *
	{
		0b00010100,
		0b00001000,
		0b00111110,
		0b00001000,
		0b00010100
	},
	// +
	{
		0b00001000,
		0b00001000,
		0b00111110,
		0b00001000,
		0b00001000,
	},
	// ,
	{
		0b00000000,
		0b01010000,
		0b00110000,
		0b00000000,
		0b00000000
	},
	// -
	{
		0b00001000,
		0b00001000,
		0b00001000,
		0b00001000,
		0b00001000
	},
	// .
	{
		0b00000000,
		0b01100000,
		0b01100000,
		0b00000000,
		0b00000000
	},
	// /
	{
		0b00100000,
		0b00010000,
		0b00001000,
		0b00000100,
		0b00000010
	},
	// 0
	{
		0b00111110,
		0b01010001,
		0b01001001,
		0b01000101,
		0b00111110
	},
	// 1
	{
		0b00000000,
		0b01000010,
		0b01111111,
		0b01000000,
		0b00000000
	},
	// 2
	{
		0b01000010,
		0b01100001,
		0b01010001,
		0b01001001,
		0b01000110
	},
	// 3
	{
		0b00100001,
		0b01000001,
		0b01000101,
		0b01001011,
		0b00110001
	},
	// 4
	{
		0b00011000,
		0b00010100,
		0b00010010,
		0b01111111,
		0b00010000
	},
	// 5
	{
		0b00100111,
		0b01000101,
		0b01000101,
		0b01000101,
		0b00111001
	},
	// 6
	{
		0b00111100,
		0b01001010,
		0b01001001,
		0b01001001,
		0b00110000
	},
	// 7
	{
		0b00000001,
		0b01110001,
		0b00001001,
		0b00000101,
		0b00000011
	},
	// 8
	{
		0b00110110,
		0b01001001,
		0b01001001,
		0b01001001,
		0b00110110
	},
	// 9
	{
		0b00000110,
		0b01001001,
		0b01001001,
		0b00101001,
		0b00011110
	},
	// :
	{
		0b00000000,
		0b00110110,
		0b00110110,
		0b00000000,
		0b00000000
	},
	// ;
	{
		0b00000000,
		0b01010110,
		0b00110110,
		0b00000000,
		0b00000000
	},
	// <
	{
		0b00001000,
		0b00010100,
		0b00100010,
		0b01000001,
		0b00000000
	},
	// =
	{
		0b00010100,
		0b00010100,
		0b00010100,
		0b00010100,
		0b00010100
	},
	// >
	{
		0b00000000,
		0b01000001,
		0b00100010,
		0b00010100,
		0b00001000
	},
	// ?
	{
		0b00000010,
		0b00000001,
		0b01010001,
		0b00001001,
		0b00000110
	},
	// @
	{
		0b00110010,
		0b01001001,
		0b01111001,
		0b01000001,
		0b00111110
	},
	// A
	{
		0b01111110,
		0b00010001,
		0b00010001,
		0b00010001,
		0b01111110
	},
	// B
	{
		0b01111111,
		0b01001001,
		0b01001001,
		0b01001001,
		0b00110110
	},
	// C
	{
		0b00111110,
		0b01000001,
		0b01000001,
		0b01000001,
		0b00100010
	},
	// D
	{
		0b01111111,
		0b01000001,
		0b01000001,
		0b00100010,
		0b00011100
	},
	// E
	{
		0b01111111,
		0b01001001,
		0b01001001,
		0b01001001,
		0b01000001
	},
	// F
	{
		0b01111111,
		0b00001001,
		0b00001001,
		0b00001001,
		0b00000001
	},
	// G
	{
		0b00111110,
		0b01000001,
		0b01001001,
		0b01001001,
		0b01111010
	},
	// H
	{
		0b01111111,
		0b00001000,
		0b00001000,
		0b00001000,
		0b01111111
	},
	// I
	{
		0b00000000,
		0b01000001,
		0b01111111,
		0b01000001,
		0b00000000
	},
	// J
	{
		0b00100000,
		0b01000000,
		0b01000001,
		0b00111111,
		0b00000001
	},
	// K
	{
		0b01111111,
		0b00001000,
		0b00010100,
		0b00100010,
		0b01000001
	},
	// L
	{
		0b01111111,
		0b01000000,
		0b01000000,
		0b01000000,
		0b01000000
	},
	// M
	{
		0b01111111,
		0b00000010,
		0b00001100,
		0b00000010,
		0b01111111
	},
	// N
	{
		0b01111111,
		0b00000100,
		0b00001000,
		0b00010000,
		0b01111111
	},
	// O
	{
		0b00111110,
		0b01000001,
		0b01000001,
		0b01000001,
		0b00111110
	},
	// P
	{
		0b01111111,
		0b00001001,
		0b00001001,
		0b00001001,
		0b00000110
	},
	// Q
	{
		0b00111110,
		0b01000001,
		0b01010001,
		0b00100001,
		0b01011110
	},
	// R
	{
		0b01111111,
		0b00001001,
		0b00011001,
		0b00101001,
		0b01000110
	},
	// S
	{
		0b01000110,
		0b01001001,
		0b01001001,
		0b01001001,
		0b00110001
	},
	// T
	{
		0b00000001,
		0b00000001,
		0b01111111,
		0b00000001,
		0b00000001
	},
	// U
	{
		0b00111111,
		0b01000000,
		0b01000000,
		0b01000000,
		0b00111111
	},
	// V
	{
		0b00011111,
		0b00100000,
		0b01000000,
		0b00100000,
		0b00011111
	},
	// W
	{
		0b00111111,
		0b01000000,
		0b00111000,
		0b01000000,
		0b00111111
	},
	// X
	{
		0b01100011,
		0b00010100,
		0b00001000,
		0b00010100,
		0b01100011
	},
	// Y
	{
		0b00000111,
		0b00001000,
		0b01110000,
		0b00001000,
		0b00000111
	},
	// Z
	{
		0b01100001,
		0b01010001,
		0b01001001,
		0b01000101,
		0b01000011
	},
};

static const uint8_t normal_char_placeholder[TEXT_NORMAL_CHAR_WIDTH] = {
	0b01111111,
	0b01010101,
	0b01001001,
	0b01010101,
	0b01111111
};

#define SPEC_CHAR_7SEG_LARGE_WIDTH 12
#define SPEC_CHAR_7SEG_LARGE_PAGES 3
static const uint8_t spec_char_7seg_large[11][SPEC_CHAR_7SEG_LARGE_PAGES][SPEC_CHAR_7SEG_LARGE_WIDTH] = {
	{
		// 0, Page 0
		{
			0b11111110,
			0b11111101,
			0b11111011,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b11111011,
			0b11111101,
			0b11111110
		},
		// 0, Page 1
		{
			0b11111111,
			0b11110111,
			0b11100011,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b11100011,
			0b11110111,
			0b11111111
		},
		// 0, Page 2
		{
			0b00111111,
			0b01011111,
			0b01101111,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01101111,
			0b01011111,
			0b00111111
		}
	},
	{
		// 1, Page 0
		{
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b11111000,
			0b11111100,
			0b11111110
		},
		// 1, Page 1
		{
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b11100011,
			0b11110111,
			0b11111111
		},
		// 1, Page 2
		{
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00001111,
			0b00011111,
			0b00111111
		}
	},
	{
		// 2, Page 0
		{
			0b00000000,
			0b00000001,
			0b00000011,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b11111011,
			0b11111101,
			0b11111110
		},
		// 2, Page 1
		{
			0b11111000,
			0b11110100,
			0b11101100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011011,
			0b00010111,
			0b00001111
		},
		// 2, Page 2
		{
			0b00111111,
			0b01011111,
			0b01101111,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01100000,
			0b01000000,
			0b00000000
		}
	},
	{
		// 3, Page 0
		{
			0b00000000,
			0b00000001,
			0b00000011,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b11111011,
			0b11111101,
			0b11111110
		},
		// 3, Page 1
		{
			0b00000000,
			0b00000000,
			0b00001000,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b11101011,
			0b11110111,
			0b11111111
		},
		// 3, Page 2
		{
			0b00000000,
			0b01000000,
			0b01100000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01101111,
			0b01011111,
			0b00111111
		}
	},
	{
		// 4, Page 0
		{
			0b11111110,
			0b11111100,
			0b11111000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b11111000,
			0b11111100,
			0b11111110
		},
		// 4, Page 1
		{
			0b00001111,
			0b00010111,
			0b00011011,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b11101011,
			0b11110111,
			0b11111111
		},
		// 4, Page 2
		{
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00001111,
			0b00011111,
			0b00111111
		}
	},
	{
		// 5, Page 0
		{
			0b11111110,
			0b11111101,
			0b11111011,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000011,
			0b00000001,
			0b00000000
		},
		// 5, Page 1
		{
			0b00001111,
			0b00010111,
			0b00011011,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b11101100,
			0b11110100,
			0b11111000
		},
		// 5, Page 2
		{
			0b00000000,
			0b01000000,
			0b01100000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01101111,
			0b01011111,
			0b00111111
		}
	},
	{
		// 6, Page 0
		{
			0b11111110,
			0b11111101,
			0b11111011,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000011,
			0b00000001,
			0b00000000
		},
		// 6, Page 1
		{
			0b11111111,
			0b11110111,
			0b11101011,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b11101100,
			0b11110100,
			0b11111000
		},
		// 6, Page 2
		{
			0b00111111,
			0b01011111,
			0b01101111,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01101111,
			0b01011111,
			0b00111111
		}
	},
	{
		// 7, Page 0
		{
			0b00000000,
			0b00000001,
			0b00000011,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b11111011,
			0b11111101,
			0b11111110
		},
		// 7, Page 1
		{
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b11100011,
			0b11110111,
			0b11111111
		},
		// 7, Page 2
		{
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00001111,
			0b00011111,
			0b00111111
		}
	},
	{
		// 8, Page 0
		{
			0b11111110,
			0b11111101,
			0b11111011,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b11111011,
			0b11111101,
			0b11111110
		},
		// 8, Page 1
		{
			0b11111111,
			0b11110111,
			0b11101011,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b11101011,
			0b11110111,
			0b11111111
		},
		// 8, Page 2
		{
			0b00111111,
			0b01011111,
			0b01101111,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01101111,
			0b01011111,
			0b00111111
		}
	},
	{
		// 9, Page 0
		{
			0b11111110,
			0b11111101,
			0b11111011,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b11111011,
			0b11111101,
			0b11111110
		},
		// 9, Page 1
		{
			0b00001111,
			0b00010111,
			0b00011011,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b00011100,
			0b11101011,
			0b11110111,
			0b11111111
		},
		// 9, Page 2
		{
			0b00000000,
			0b01000000,
			0b01100000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01101111,
			0b01011111,
			0b00111111
		}
	},
	{
		// C, Page 0
		{
			0b11111110,
			0b11111101,
			0b11111011,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000111,
			0b00000011,
			0b00000001,
			0b00000000
		},
		// C, Page 1
		{
			0b11111111,
			0b11110111,
			0b11100011,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000,
			0b00000000
		},
		// C, Page 2
		{
			0b00111111,
			0b01011111,
			0b01101111,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01110000,
			0b01100000,
			0b01000000,
			0b00000000
		}
	},
};

void text_draw(const char* s, uint8_t line, uint8_t x, uint8_t y_offset)
{
	uint8_t length = strlen(s);

	if (length == 0 || line > 7 || x > 127 || y_offset > 7)
		return;

	ssd1306_page_addressing();
	ssd1306_set_page(line);

	for (uint8_t i = 0; i < length; ++i) {
		ssd1306_set_start_column(x);
		x += TEXT_NORMAL_CHAR_WIDTH + 1;

		// Stop if the next character won't fit
		if (x > SSD1306_LCDWIDTH - 1)
			return;

		char c = s[i];
		const uint8_t* char_data;

		// If character is not supported, draw placeholder
		if (c >= sizeof(normal_charset)) {
			char_data = normal_char_placeholder;
		} else {
			// Get data for the next character
			char_data = normal_charset[c - 32];
		}

		// Send data to the display
		ssd1306_send_data(char_data, TEXT_NORMAL_CHAR_WIDTH, y_offset);
	}
}

void text_draw_7seg_large(const char* number, uint8_t line, uint8_t x)
{
	uint8_t length = strlen(number);

	if (length == 0 || line > (7 - SPEC_CHAR_7SEG_LARGE_PAGES) || x > 127)
		return;

	ssd1306_page_addressing();

	for (uint8_t i = 0; i < length; ++i) {
		// Stop if the next character won't fit
		if (x + SPEC_CHAR_7SEG_LARGE_WIDTH + 1 > SSD1306_LCDWIDTH - 1)
			return;

		char c = number[i];

		// Space character only increments the left offset
		if (c != ' ') {
			if (c == '-') {
				ssd1306_set_start_column(x);
				ssd1306_set_page(line + 1);

				// Cost-efficient dash symbol
				uint8_t char_data = 0b00011100;
				for (uint8_t j = 2; j < SPEC_CHAR_7SEG_LARGE_WIDTH - 2; ++j)
					ssd1306_send_data(&char_data, 1, 0);
			} else {
				// Draw the pages of the character
				for (uint8_t page = 0; page < SPEC_CHAR_7SEG_LARGE_PAGES; ++page) {
					ssd1306_set_page(line + page);
					ssd1306_set_start_column(x);

					const uint8_t* char_data = 0;

					if (c >= '0' && c <= '9')
						char_data = spec_char_7seg_large[c - '0'][page];
					else if (c == 'C' || c == 'c')
						char_data = spec_char_7seg_large[9 + 1][page];

					ssd1306_send_data(char_data, SPEC_CHAR_7SEG_LARGE_WIDTH, 0);
				}
			}
		} else {
			for (uint8_t page = 0; page < SPEC_CHAR_7SEG_LARGE_PAGES; ++page) {
				ssd1306_set_page(line + page);
				ssd1306_set_start_column(x);

				uint8_t char_data[SPEC_CHAR_7SEG_LARGE_WIDTH] = { 0 };
				ssd1306_send_data(char_data, sizeof(char_data), 0);
			}
		}

		x += SPEC_CHAR_7SEG_LARGE_WIDTH + 2;
	}
}