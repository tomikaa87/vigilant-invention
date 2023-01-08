#include "graphics.h"
#include "sh1106.h"

const uint8_t Bitmap_cloud1[BITMAP_CLOUD1_WIDTH * BITMAP_CLOUD1_PAGES] = {
    0b10000000, 0b11100000, 0b11100000, 0b11110000, 0b11110000, 0b11111000, 0b11111110, 0b11111110,
    0b11111111, 0b11111111, 0b11111111, 0b11111110, 0b11111110, 0b11111000, 0b11111110, 0b11111110,
    0b11111111, 0b11111111, 0b11111110, 0b11111110, 0b11111100, 0b11111000, 0b11111000, 0b11110000,
    0b11000000,
    0b00000011, 0b00001111, 0b00001111, 0b00011111, 0b00011111, 0b00011111, 0b00001111, 0b00001111,
    0b00011111, 0b00011111, 0b00111111, 0b00111111, 0b00111111, 0b00011111, 0b00011111, 0b00000111,
    0b00001111, 0b00001111, 0b00011111, 0b00011111, 0b00011111, 0b00001111, 0b00001111, 0b00000111,
    0b00000001
};

const uint8_t Bitmap_snowman1[BITMAP_SNOWMAN1_WIDTH * BITMAP_SNOWMAN1_PAGES] = {
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11100100, 0b11110111, 0b11011101, 0b01111101,
    0b11011101, 0b11110111, 0b11100100, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000001, 0b00000010, 0b11000100, 0b11101011, 0b10011100, 0b00111001, 0b01111011, 0b01010011,
    0b01111011, 0b00111001, 0b10011100, 0b11101011, 0b11000100, 0b00000010, 0b00000001,
    0b00000000, 0b00000000, 0b00000011, 0b00000111, 0b00001111, 0b00011111, 0b00011111, 0b00011101,
    0b00011111, 0b00011111, 0b00001111, 0b00000111, 0b00000011, 0b00000000, 0b00000000
};

const uint8_t Bitmap_snowman1Mask[BITMAP_SNOWMAN1_MASK_WIDTH * BITMAP_SNOWMAN1_MASK_PAGES] = {
    0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111111, 0b11111111, 0b11111111,
    0b11111111,
    0b11111000, 0b11110000, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100000, 0b11110000,
    0b11111000,
    0b11111111, 0b11111111, 0b11110000, 0b11100000, 0b11000000, 0b10000000, 0b10000000, 0b10000000,
    0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b11000000, 0b11100000, 0b11110000, 0b11111111,
    0b11111111
};

void Graphics_drawBitmap(
    uint8_t* const frameBuffer,
    const uint8_t x,
    const uint8_t y,
    const uint8_t width,
    const uint8_t height,
    const uint8_t* const bitmap,
    const bool mask
) {
    uint8_t alignmentOffset = y & 0b111;
    uint8_t startPage = y >> 3;
    uint8_t pageCount = height / 8 + (height % 8 > 0 ? 1 : 0);
    uint8_t extraPages = alignmentOffset > 0 ? 1 : 0;

    for (uint8_t page = 0u; page < pageCount + extraPages && page < SH1106_PAGES; ++page) {
        for (uint8_t col = x; col < x + width && col < SH1106_WIDTH; ++col) {
            uint8_t* fbByte = &frameBuffer[col + (startPage + page) * SH1106_WIDTH];

            if (page < pageCount) {
                uint8_t b = (uint8_t)(bitmap[page * width + col - x] << alignmentOffset);
                if (!mask) {
                    *fbByte |= b;
                } else {
                    *fbByte &= b | ~(0xff << alignmentOffset);
                }
            }

            if (page > 0 && alignmentOffset > 0) {
                uint8_t b = (uint8_t)(bitmap[(page - 1) * width + col - x] >> (8 - alignmentOffset));
                if (!mask) {
                    *fbByte |= b;
                } else {
                    *fbByte &= b | ~(0xff >> (8 - alignmentOffset));
                }
            }
        }
    }
}

uint8_t Graphics_drawBitmapBufferless(
    uint8_t block,
    const uint8_t page,
    const uint8_t col,
    const uint8_t x,
    const uint8_t y,
    const uint8_t w,
    const uint8_t h,
    const uint8_t* const bitmap,
    const bool mask
) {
    uint8_t startPage = y >> 3;
    uint8_t alignmentOffset = y & 0b111;
    uint8_t pageCount = h / 8 + (h % 8 > 0 ? 1 : 0);
    uint8_t endPage = startPage + pageCount + (alignmentOffset > 0 ? 1 : 0);
    uint8_t extraPages = alignmentOffset > 0 ? 1 : 0;

    if (
        col < x
        || col >= x + w
        || page < startPage
        || page >= endPage
    ) {
        return block;
    }

    uint8_t relativePage = page - startPage;

    if (relativePage < pageCount) {
        uint8_t b = (uint8_t)(bitmap[relativePage * w + col - x] << alignmentOffset);
        if (!mask) {
            block |= b;
        } else {
            block &= b | ~(0xff << alignmentOffset);
        }
    }

    if (relativePage > 0 && alignmentOffset > 0) {
        uint8_t b = (uint8_t)(bitmap[(relativePage - 1) * w + col - x] >> (8 - alignmentOffset));
        if (!mask) {
            block |= b;
        } else {
            block &= b | ~(0xff >> (8 - alignmentOffset));
        }
    }

    return block;
}