/*
 * graphics.h
 *
 *  Created on: 2018. dec. 30.
 *      Author: tomikaa
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_


#include <stdint.h>


void Graphics_DrawCursor(uint8_t x, uint8_t y);
void Graphics_UpdateScreen();

void Graphics_SetPixel(uint8_t x, uint8_t y);

#endif /* GRAPHICS_H_ */
