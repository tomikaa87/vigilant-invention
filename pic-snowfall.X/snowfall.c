#include "sh1106.h"
#include "snowfall.h"

#include "mcc_generated_files/mcc.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <xc.h>

#define SNOWFLAKE_COUNT                 100u
#define SNOW_PILE_MAX_HEIGHT_PAGES      3u
#define SNOW_PILE_REDUCE_MIN_HEIGH      5u
#define CLOUD_COUNT                     5u

// Snowflake:
// | 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0 |
// |  R  R  F  Y  Y  Y  Y  Y  Y  X  X  X  X  X  X  X |
// X: X-coordinate
// Y: Y-coordinate
// R: reserved
// F: "falling" bit

// Snow pile
/*
 *      x
 * o -> o -> ?o?
 *
 *       x
 * oo -> oo -> ooo
 *
 *        x
 * oo -> oo -> ooo
 *
 *       x
 * o     o      o
 * oo -> oo -> ooo
 *
 *        x
 *  o     o     o
 * oo -> oo -> ooo
 *
 *         x
 *  o      o     ?o?
 * ooo -> ooo -> ooo
 */

static struct Context
{
    uint8_t canvas[SH1106_WIDTH * SH1106_HEIGHT / 8];

    struct Snowflake
    {
        uint8_t x: 7;
        uint8_t y: 6;
        uint8_t falling: 1;
        uint8_t reserved: 2;
    } snowflakes[SNOWFLAKE_COUNT];

    struct Cloud
    {
        uint8_t x;
        uint8_t y;
    } clouds[CLOUD_COUNT];

    uint8_t snowPileHeights[SH1106_WIDTH]; // TODO optimize the memory usage of this

    volatile bool updateClouds;
} context = {
};

uint8_t pattern = 0;

static void drawBitmap(
    const uint8_t x,
    const uint8_t y,
    const uint8_t width,
    const uint8_t height,
    const uint8_t* const bitmap,
    const bool mask
);

static inline uint8_t umin(const uint8_t a, const uint8_t b)
{
    return a < b ? a : b;
}

static inline uint8_t umax(const uint8_t a, const uint8_t b)
{
    return a > b ? a : b;
}

static inline uint8_t smin(const int8_t a, const int8_t b)
{
    return a < b ? a : b;
}

static inline uint8_t smax(const int8_t a, const int8_t b)
{
    return a > b ? a : b;
}

static bool advanceLogic(void);
static bool getCanvasPixel(const uint8_t x, const uint8_t y);
static bool addSnowflakeToPile(const uint8_t x, const uint8_t y);
static uint8_t getSnowPileHeight(const uint8_t x);
static void setSnowPileHeight(const uint8_t x, const uint8_t height);
static void timer0InterruptHandler(void);

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


void Snowfall_init()
{
    SH1106_init();
    SH1106_setPowerOn(true);
    SH1106_setContrast(0x20);
    SH1106_fill(0xFF);
    __delay_ms(1000);
    SH1106_fill(0);

    memset(&context, 0, sizeof(context));

    TMR0_SetInterruptHandler(timer0InterruptHandler);

    for (uint8_t i = 0; i < SNOWFLAKE_COUNT; ++i) {
        context.snowflakes[i].x = rand() % 128;
        context.snowflakes[i].y = rand() % 64;
    }

    for (uint8_t i = 0; i < CLOUD_COUNT; ++i) {
        context.clouds[i].x = i * (SH1106_WIDTH / CLOUD_COUNT);
        context.clouds[i].y = rand() % 3;
    }
}

void Snowfall_task()
{
    memset(context.canvas, 0, sizeof(context.canvas));

#if 0
    static uint8_t x = 0;
    static uint8_t x2 = 20;

    drawBitmap(
        x, 0,
        BITMAP_CLOUD1_WIDTH,
        BITMAP_CLOUD1_HEIGHT,
        Bitmap_cloud1,
        false
    );

        drawBitmap(
        x2, 10,
        BITMAP_CLOUD1_WIDTH,
        BITMAP_CLOUD1_HEIGHT,
        Bitmap_cloud1,
        false
    );


    if (++x >= SH1106_WIDTH - BITMAP_CLOUD1_WIDTH) {
        x = 0;
    }

    if (++x2 >= SH1106_WIDTH - BITMAP_CLOUD1_WIDTH) {
        x2 = 0;
    }
#endif

    bool update = advanceLogic();

    if (update) {
        // Draw the clouds
        for (uint8_t i = 0; i < CLOUD_COUNT; ++i) {
            drawBitmap(
                context.clouds[i].x,
                context.clouds[i].y,
                BITMAP_CLOUD1_WIDTH,
                BITMAP_CLOUD1_HEIGHT,
                Bitmap_cloud1,
                false
            );
        }

        // Draw the snow piles
        for (uint8_t x = 0; x < SH1106_WIDTH; ++x) {
            uint8_t height = getSnowPileHeight(x);

            if (height == 0) {
                continue;
            }

            uint8_t page = 7;

            while (height >> 3 > 0) {
                context.canvas[x + page * SH1106_WIDTH] = 0xFF;
                height -= 8;
                page -= 1;
            }

            context.canvas[x + page * SH1106_WIDTH] = 0xFF << (8 - height);
        }

        // Draw the snowflakes on the canvas
        for (uint8_t i = 0; i < SNOWFLAKE_COUNT; ++i) {
            struct Snowflake s = context.snowflakes[i];
//            if (s.falling) {
                context.canvas[s.x + (s.y >> 3) * SH1106_WIDTH] |= 1 << (s.y & 0b111);
//            }
        }

        // Draw the snowman and its mask
        uint8_t snowmanX = 20u, snowmanY = SH1106_HEIGHT - 25;
        drawBitmap(
            snowmanX - 1,
            snowmanY - 1,
            BITMAP_SNOWMAN1_MASK_WIDTH,
            BITMAP_SNOWMAN1_MASK_HEIGHT,
            Bitmap_snowman1Mask,
            true
        );
        drawBitmap(
            snowmanX,
            snowmanY,
            BITMAP_SNOWMAN1_WIDTH,
            BITMAP_SNOWMAN1_HEIGHT,
            Bitmap_snowman1,
            false
        );

        // Draw the canvas on the display
        for (uint8_t page = 0; page < SH1106_PAGES; ++page) {
            SH1106_setColumn(0);
            SH1106_setLine(page);
            SH1106_sendDataArray(
                context.canvas + (page * SH1106_WIDTH),
                SH1106_WIDTH,
                0,
                false
            );
        }
    }
}

static void drawBitmap(
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
            uint8_t* fbByte = &context.canvas[col + (startPage + page) * SH1106_WIDTH];

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



static bool advanceLogic()
{
    bool reducePileHeights = false;
    for (uint8_t i = 0; i < SH1106_WIDTH; ++i) {
        if (context.snowPileHeights[i] >= SNOW_PILE_MAX_HEIGHT_PAGES * 8) {
            reducePileHeights = true;
            break;
        }
    }
    if (reducePileHeights) {
        for (uint8_t i = 0; i < SH1106_WIDTH; ++i) {
            if (context.snowPileHeights[i] > SNOW_PILE_REDUCE_MIN_HEIGH) {
                --context.snowPileHeights[i];
            }
        }
    }

    for (uint8_t i = 0; i < SNOWFLAKE_COUNT; ++i) {
        struct Snowflake* s = &context.snowflakes[i];

        if (addSnowflakeToPile(s->x, s->y)) {
            s->x = rand() % 128;
            s->y = rand() % 5 + 8;
        } else {
            if (s->y == SH1106_HEIGHT - 1) {
                s->x = rand() % 128;
                s->y = rand() % 5 + 8;
            } else {
                ++s->y;
                uint8_t r = rand() % 2;
                if (r == 0) {
                    s->x = s->x == 0 ? 127 : s->x - 1;
                } else {
                    s->x = s->x == 127 ? 0 : s->x + 1;
                }
            }
        }
    }

    if (context.updateClouds) {
        context.updateClouds = false;
        for (uint8_t i = 0; i < CLOUD_COUNT; ++i) {
            int8_t rangeMin = i * (SH1106_WIDTH / CLOUD_COUNT);
            int8_t rangeMax = (i + 1) * (SH1106_WIDTH / CLOUD_COUNT);
            int8_t rangeCenter = rangeMin + (rangeMax - rangeMin) / 2;

//            context.canvas[rangeMin] = 0xff;
//            context.canvas[rangeCenter] = 0x55;

            context.clouds[i].x = smax(0, smin(SH1106_WIDTH - BITMAP_CLOUD1_WIDTH, rangeCenter + rand() % 5 - 2 - BITMAP_CLOUD1_WIDTH / 2));
            context.clouds[i].y = rand() % 3;
        }
    }

    return true;
}

static bool getCanvasPixel(const uint8_t x, const uint8_t y)
{
    return context.canvas[x + (y >> 3) * SH1106_WIDTH] & 1 << (y & 0b111);
}

static bool addSnowflakeToPile(const uint8_t x, const uint8_t y)
{
    if (
        x >= SH1106_WIDTH
        || y < (SH1106_HEIGHT - SNOW_PILE_MAX_HEIGHT_PAGES * 8)
    ) {
        return false;
    }

    uint8_t height = getSnowPileHeight(x);

    if (y < (SH1106_HEIGHT - height - 1)) {
        return false;
    }

    // Empty column, place instantly
    if (height == 0) {
        setSnowPileHeight(x, height + 1);
        return true;
    }

    uint8_t leftColumn = x == 0 ? 127 : x - 1;
    uint8_t rightColumn = x == 127 ? 0 : x + 1;

    uint8_t leftHeight = getSnowPileHeight(leftColumn);
    uint8_t rightHeight = getSnowPileHeight(rightColumn);

    if (leftHeight >= (height - 0) && rightHeight >= (height - 0)) {
        setSnowPileHeight(x, height + 1);
        return true;
    }

    //
//    if (leftHeight == rightHeight && leftHeight == )
#if 0
    uint8_t radius = 1;
//    bool checkLeftSide = rand() % 2;
//    bool leftSideChecked = false;
//    bool rightSideChecked = false;

    while (true) {
        uint8_t leftHeight = 255;
        uint8_t leftColumn = 255;
        if (radius <= x) {
            leftColumn = x - radius;
            leftHeight = getSnowPileHeight(leftColumn);
        }

        uint8_t rightHeight = 255;
        uint8_t rightColumn = 255;
        if (radius + x < SH1106_WIDTH) {
            rightColumn = radius + x;
            rightHeight = getSnowPileHeight(rightColumn);
        }

        if (leftHeight == 0 && rightHeight == 0) {
            bool left = rand() % 2 == 0;
            setSnowPileHeight(
                left ? leftColumn : rightColumn,
                left ? leftHeight + 1 : rightHeight + 1
            );
            return true;
        }

        if (leftHeight == 0) {
            setSnowPileHeight(leftColumn, leftHeight + 1);
            return true;
        }

        if (rightHeight == 0) {
            setSnowPileHeight(rightColumn, rightHeight + 1);
            return true;
        }

        // Can be placed on the top of the center pile without making a tower?
        if (leftHeight == rightHeight && leftHeight < (height + 1) && (height + 1) - leftHeight < 2) {
            setSnowPileHeight(x, height + 1);
            return true;
        }

        if (leftHeight < (height + 1) && (height + 1) - leftHeight < 2)

        break;

//        if (checkLeftSide && !leftSideChecked) {
//            leftSideChecked = true;
//            checkLeftSide = false;
//        } else if (!checkLeftSide && !rightSideChecked) {
//            rightSideChecked = true;
//            checkLeftSide = true;
//        } else {
//            // end
//        }
    }
#endif

    return false;
}

static uint8_t getSnowPileHeight(const uint8_t x)
{
    return context.snowPileHeights[x];
}

static void setSnowPileHeight(const uint8_t x, const uint8_t height)
{
    context.snowPileHeights[x] = height;
}

static void timer0InterruptHandler()
{
    context.updateClouds = true;
}

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