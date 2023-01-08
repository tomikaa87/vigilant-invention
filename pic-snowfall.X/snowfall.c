#include "graphics.h"
#include "sh1106.h"
#include "snowfall.h"

#include "mcc_generated_files/mcc.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <xc.h>

#define SNOWFLAKE_COUNT                 100u
#define SNOW_PILE_MAX_HEIGHT            15u
#define SNOW_PILE_REDUCE_MIN_HEIGH      5u
#define CLOUD_COUNT                     5u

typedef struct
{
    uint8_t even : 4;
    uint8_t odd : 4;
} SnowPileHeight;

static struct Context
{
    uint8_t canvas[SH1106_WIDTH * SH1106_HEIGHT / 8];

    struct Snowflake
    {
        uint8_t x;
        uint8_t y;
    } snowflakes[SNOWFLAKE_COUNT];

    struct Cloud
    {
        uint8_t x;
        uint8_t y;
    } clouds[CLOUD_COUNT];

    // Height of a single pile can be stored in 4 bits
    SnowPileHeight snowPileHeights[SH1106_WIDTH / 2];

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
                context.canvas[s.x + (s.y >> 3) * SH1106_WIDTH] |= 1 << (s.y & 0b111);
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
        if (getSnowPileHeight(i) >= SNOW_PILE_MAX_HEIGHT) {
            reducePileHeights = true;
            break;
        }
    }
    if (reducePileHeights) {
        for (uint8_t i = 0; i < SH1106_WIDTH; ++i) {
            uint8_t height = getSnowPileHeight(i);
            if (height > SNOW_PILE_REDUCE_MIN_HEIGH) {
                setSnowPileHeight(i, height - 1);
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
        || y < (SH1106_HEIGHT - SNOW_PILE_MAX_HEIGHT)
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

    return false;
}

static uint8_t getSnowPileHeight(const uint8_t x)
{
    SnowPileHeight h = context.snowPileHeights[x >> 1];
    return x & 0b1 ? h.odd : h.even;
}

static void setSnowPileHeight(const uint8_t x, const uint8_t height)
{
    if (x & 0b1) {
        context.snowPileHeights[x >> 1].odd = height;
    } else {
        context.snowPileHeights[x >> 1].even = height;
    }
}

static void timer0InterruptHandler()
{
    context.updateClouds = true;
}