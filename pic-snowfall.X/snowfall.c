#include "graphics.h"
#include "sh1106.h"
#include "snowfall.h"

#include "mcc_generated_files/mcc.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <xc.h>

#define USE_FRAME_BUFFER                0
#define SNOWFLAKE_COUNT                 50u
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
#if USE_FRAME_BUFFER
    uint8_t frameBuffer[SH1106_WIDTH * SH1106_HEIGHT / 8];
#else
    uint8_t snowflakeColIndicators[SH1106_WIDTH / 8];
#endif
    
//    uint8_t snowflakeXBits[128 / 8];
//    uint8_t snowflakeYBits[64 / 8];

    struct Snowflake
    {
        uint8_t x;
        uint8_t y;
    } snowflakes[SNOWFLAKE_COUNT];

#if USE_FRAME_BUFFER
    struct Cloud
    {
        uint8_t x;
        uint8_t y;
    } clouds[CLOUD_COUNT];
#else
    BufferlessBitmapInfo clouds[CLOUD_COUNT];
#endif

    // Height of a single pile can be stored in 4 bits
    SnowPileHeight snowPileHeights[SH1106_WIDTH / 2];

    volatile bool updateClouds;
    
    BufferlessBitmapInfo snowman;
    BufferlessBitmapInfo snowmanMask;
} context;

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

static void advanceLogic(void);
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
    
    context.updateClouds = true;
    
    context.snowman = Graphics_precalculateBufferlessBitmap(
        20,
        SH1106_HEIGHT - 25,
        BITMAP_SNOWMAN1_WIDTH,
        BITMAP_SNOWMAN1_HEIGHT,
        false
    );
    
    context.snowmanMask = Graphics_precalculateBufferlessBitmap(
        20 - 1,
        SH1106_HEIGHT - 25 - 1,
        BITMAP_SNOWMAN1_MASK_WIDTH,
        BITMAP_SNOWMAN1_MASK_HEIGHT,
        true
    );

    TMR0_SetInterruptHandler(timer0InterruptHandler);

    for (uint8_t i = 0; i < SNOWFLAKE_COUNT; ++i) {
        context.snowflakes[i].x = rand() % 128;
        context.snowflakes[i].y = rand() % 64;
    }

//    for (uint8_t i = 0; i < CLOUD_COUNT; ++i) {
//        context.clouds[i].x = i * (SH1106_WIDTH / CLOUD_COUNT);
//        context.clouds[i].y = rand() % 3;
//    }
}

void Snowfall_task()
{
    advanceLogic();

#if USE_FRAME_BUFFER
    memset(context.frameBuffer, 0, sizeof(context.frameBuffer));

    // Draw the clouds
    for (uint8_t i = 0; i < CLOUD_COUNT; ++i) {
        Graphics_drawBitmap(
            context.frameBuffer,
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
            context.frameBuffer[x + page * SH1106_WIDTH] = 0xFF;
            height -= 8;
            page -= 1;
        }

        context.frameBuffer[x + page * SH1106_WIDTH] = 0xFF << (8 - height);
    }

    // Draw the snowflakes on the canvas
    for (uint8_t i = 0; i < SNOWFLAKE_COUNT; ++i) {
        struct Snowflake s = context.snowflakes[i];
            context.frameBuffer[s.x + (s.y >> 3) * SH1106_WIDTH] |= 1 << (s.y & 0b111);
    }

    // Draw the snowman and its mask
    uint8_t snowmanX = 20u, snowmanY = SH1106_HEIGHT - 25;
    Graphics_drawBitmap(
        context.frameBuffer,
        snowmanX - 1,
        snowmanY - 1,
        BITMAP_SNOWMAN1_MASK_WIDTH,
        BITMAP_SNOWMAN1_MASK_HEIGHT,
        Bitmap_snowman1Mask,
        true
    );
    Graphics_drawBitmap(
        context.frameBuffer,
        snowmanX,
        snowmanY,
        BITMAP_SNOWMAN1_WIDTH,
        BITMAP_SNOWMAN1_HEIGHT,
        Bitmap_snowman1,
        false
    );

    // Send the frame buffer to the display
    for (uint8_t page = 0; page < SH1106_PAGES; ++page) {
        SH1106_setColumn(0);
        SH1106_setLine(page);
        SH1106_sendDataArray(
            context.frameBuffer + (page * SH1106_WIDTH),
            SH1106_WIDTH,
            0,
            false
        );
    }
#else
    for (uint8_t page = 0; page < SH1106_PAGES; ++page) {
        SH1106_setLine(page);
        SH1106_setColumn(0);

        for (uint8_t col = 0; col < SH1106_WIDTH; ++col) {

            uint8_t block = 0;

            // Draw the clouds
            if (page < 3){
                for (uint8_t i = 0; i < CLOUD_COUNT; ++i) {
                    block = Graphics_drawPrecalculatedBufferlessBitmap(
                        block,
                        page,
                        col,
                        &context.clouds[i],
                        Bitmap_cloud1
                    );
                }
            }

            // Draw the snow piles
            if (page >= 6) {
                uint8_t pileHeight = getSnowPileHeight(col);
                if (pileHeight > 0) {
                    // FIXME: only 2-page tall piles are supported
                    if (pileHeight > 8) {
                        if (page == 6) {
                            block |= 0xFF << (8 - pileHeight);
                        } else if (page == 7) {
                            block |= 0xFF;
                        }
                    } else if (page == 7) {
                        block |= 0xFF << (8 - pileHeight);
                    }
                }                
            }

            // Draw the snowflakes
            // FIXME this slows down things significantly
            // Check for snowflakes only if the indicator bit for the column is set
            if (context.snowflakeColIndicators[col >> 3] & (1 << (col & 0b111))) {
                for (uint8_t i = 0; i < SNOWFLAKE_COUNT; ++i) {
                    struct Snowflake s = context.snowflakes[i];
                    if (s.x != col) {
                        continue;
                    }
                    // Do this slow check later
                    if (s.y >> 3 != page) {
                        continue;
                    }
                    block |= 1 << (s.y & 0b111);
                }
            }

            // Draw the snowman and its mask
            block = Graphics_drawPrecalculatedBufferlessBitmap(
                block, 
                page, 
                col, 
                &context.snowmanMask,
                Bitmap_snowman1Mask
            );
            block = Graphics_drawPrecalculatedBufferlessBitmap(
                block, 
                page, 
                col, 
                &context.snowman,
                Bitmap_snowman1
            );

            SH1106_sendData(block, 0, false);
        }
    }
#endif
}

static void advanceLogic()
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

#if !USE_FRAME_BUFFER
    memset(context.snowflakeColIndicators, 0, sizeof(context.snowflakeColIndicators));
#endif

    for (uint8_t i = 0; i < SNOWFLAKE_COUNT; ++i) {
        struct Snowflake* s = &context.snowflakes[i];
        
        int rnd = rand();

        if (addSnowflakeToPile(s->x, s->y)) {
            s->x = rnd % 128;
            s->y = rnd % 5 + 8;
        } else {
            if (s->y == SH1106_HEIGHT - 1) {
                s->x = rnd % 128;
                s->y = rnd % 5 + 8;
            } else {
                ++s->y;
                uint8_t r = rnd % 2;
                if (r == 0) {
                    s->x = s->x == 0 ? 127 : s->x - 1;
                } else {
                    s->x = s->x == 127 ? 0 : s->x + 1;
                }
            }
        }

        context.snowflakeColIndicators[s->x >> 3] |= 1 << (s->x & 0b111);
    }

    if (context.updateClouds) {
        context.updateClouds = false;
        for (uint8_t i = 0; i < CLOUD_COUNT; ++i) {
            int rnd = rand();
            int8_t rangeMin = i * (SH1106_WIDTH / CLOUD_COUNT);
            int8_t rangeMax = (i + 1) * (SH1106_WIDTH / CLOUD_COUNT);
            int8_t rangeCenter = rangeMin + (rangeMax - rangeMin) / 2;
            
#if USE_FRAME_BUFFER
            context.clouds[i].x = smax(0, smin(SH1106_WIDTH - BITMAP_CLOUD1_WIDTH, rangeCenter + rnd % 5 - 2 - BITMAP_CLOUD1_WIDTH / 2));
            context.clouds[i].y = rnd % 3;
#else
            context.clouds[i] = Graphics_precalculateBufferlessBitmap(
                smax(0, smin(SH1106_WIDTH - BITMAP_CLOUD1_WIDTH, rangeCenter + rnd % 5 - 2 - BITMAP_CLOUD1_WIDTH / 2)),
                rnd % 3,
                BITMAP_CLOUD1_WIDTH,
                BITMAP_CLOUD1_HEIGHT,
                false
            );            
#endif
        }
    }
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