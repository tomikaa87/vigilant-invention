/*
 * graphics.h
 *
 *  Created on: 2018. dec. 30.
 *      Author: tomikaa
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_


#include <stdint.h>


enum
{
    GRAPHICS_COLOR_WHITE,
    GRAPHICS_COLOR_LIGHTGRAY,
    GRAPHICS_COLOR_DARKGRAY,
    GRAPHICS_COLOR_BLACK
};


void Graphics_DrawCursor(uint8_t x, uint8_t y);
void Graphics_UpdateScreen();

void Graphics_SetPixel(uint8_t x, uint8_t y, uint8_t color);

#endif /* GRAPHICS_H_ */
