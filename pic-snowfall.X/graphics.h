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

typedef struct
{
    uint8_t x;
    uint8_t w;
    uint8_t startPage : 3;
    uint8_t endPage : 3;
    uint8_t reserved1 : 2;
    uint8_t pageCount : 3;
    uint8_t alignmentOffset : 3;
    uint8_t mask : 1;
    uint8_t reserved2 : 1;
} BufferlessBitmapInfo;

BufferlessBitmapInfo Graphics_precalculateBufferlessBitmap(
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    bool mask
);

uint8_t Graphics_drawPrecalculatedBufferlessBitmap(
    uint8_t block,
    uint8_t page,
    uint8_t col,
    const BufferlessBitmapInfo* info,
    const uint8_t* data
);

uint8_t Graphics_drawBitmapBufferless(
    uint8_t block,
    uint8_t page,
    uint8_t col,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    const uint8_t* bitmap,
    bool mask
);