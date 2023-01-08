/*
 * File:   graphics.h
 * Author: tomikaa
 *
 * Created on January 8, 2023, 7:58 PM
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#define BITMAP_CLOUD1_WIDTH     25
#define BITMAP_CLOUD1_HEIGHT    14
#define BITMAP_CLOUD1_PAGES     2
extern const uint8_t Bitmap_cloud1[];

#define BITMAP_SNOWMAN1_WIDTH   15
#define BITMAP_SNOWMAN1_HEIGHT  21
#define BITMAP_SNOWMAN1_PAGES   3
extern const uint8_t Bitmap_snowman1[];

#define BITMAP_SNOWMAN1_MASK_WIDTH  17
#define BITMAP_SNOWMAN1_MASK_HEIGHT 23
#define BITMAP_SNOWMAN1_MASK_PAGES  3
extern const uint8_t Bitmap_snowman1Mask[];

void Graphics_drawBitmap(
    uint8_t* const frameBuffer,
    const uint8_t x,
    const uint8_t y,
    const uint8_t width,
    const uint8_t height,
    const uint8_t* const bitmap,
    const bool mask
);