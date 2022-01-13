#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define CONFIG_USE_OLED_SH1106
#include "Display.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <tuple>

extern const uint8_t Bitmap1[20 * 3];

enum
{
    I2CSDA = 4,
    I2CSCL = 5
};

void oled_i2c_init()
{
    gpio_init(I2CSDA);
    gpio_set_function(I2CSDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2CSDA);

    gpio_init(I2CSCL);
    gpio_set_function(I2CSCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2CSCL);

    i2c_init(i2c0, 1000 * 1000);
}

//
// Logic
//

namespace Logic
{
    constexpr auto CanvasWidth = 128;
    constexpr auto CanvasHeight = 64;
    constexpr auto MaxSnowflakes = 50u;
    constexpr auto MaxClouds = 10u;

    using Canvas = std::array<bool, CanvasWidth * CanvasHeight>;

    struct Snowflake
    {
        int x = 0;
        int y = 0;
        bool falling = false;
    };

    using Snowflakes = std::array<Snowflake, MaxSnowflakes>;

    namespace Sprites {
        const std::tuple<int, int, std::array<uint8_t, 25 * 2>> Cloud1{
            25, 14,
            {
                0b10000000, 0b11100000, 0b11100000, 0b11110000, 0b11110000, 0b11111000, 0b11111110, 0b11111110,
                0b11111111, 0b11111111, 0b11111111, 0b11111110, 0b11111110, 0b11111000, 0b11111110, 0b11111110,
                0b11111111, 0b11111111, 0b11111110, 0b11111110, 0b11111100, 0b11111000, 0b11111000, 0b11110000,
                0b11000000,
                0b00000011, 0b00001111, 0b00001111, 0b00011111, 0b00011111, 0b00011111, 0b00001111, 0b00001111,
                0b00011111, 0b00011111, 0b00111111, 0b00111111, 0b00111111, 0b00011111, 0b00011111, 0b00000111,
                0b00001111, 0b00001111, 0b00011111, 0b00011111, 0b00011111, 0b00001111, 0b00001111, 0b00000111,
                0b00000001
            }
        };

        const std::tuple<int, int, std::array<uint8_t, 15 * 3>> Snowman1{
            15, 21,
            {
                0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11100100, 0b11110111, 0b11011101, 0b01111101,
                0b11011101, 0b11110111, 0b11100100, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
                0b00000001, 0b00000010, 0b11000100, 0b11101011, 0b10011100, 0b00111001, 0b01111011, 0b01010011,
                0b01111011, 0b00111001, 0b10011100, 0b11101011, 0b11000100, 0b00000010, 0b00000001,
                0b00000000, 0b00000000, 0b00000011, 0b00000111, 0b00001111, 0b00011111, 0b00011111, 0b00011101,
                0b00011111, 0b00011111, 0b00001111, 0b00000111, 0b00000011, 0b00000000, 0b00000000
            }
        };

        const std::tuple<int, int, std::array<uint8_t, 17 * 3>> Snowman1Mask{
            17, 23,
            {
                0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
                0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111111, 0b11111111, 0b11111111,
                0b11111111,
                0b11111000, 0b11110000, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
                0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100000, 0b11110000,
                0b11111000,
                0b11111111, 0b11111111, 0b11110000, 0b11100000, 0b11000000, 0b10000000, 0b10000000, 0b10000000,
                0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b11000000, 0b11100000, 0b11110000, 0b11111111,
                0b11111111
            }
        };
    }

    struct Cloud
    {
        int x = 0;
        int y = 0;

        enum class Shape
        {
            Shape1
        };

        Shape shape = Shape::Shape1;
    };

    using Clouds = std::array<Cloud, MaxClouds>;

    struct State
    {
        Canvas canvas{};
        Snowflakes snowflakes{};
        Clouds clouds{};
        unsigned ticks = 0;
    };

    bool getCanvasPixel(const Canvas& canvas, const int x, const int y)
    {
        const auto ptr = x + y * CanvasWidth;
        if (ptr < 0 || ptr >= canvas.size())
        {
            return false;
        }
        return canvas[ptr];
    }

    void advance(State& state)
    {
        int tallestHeight = 0;

        for (auto& [x, y, falling] : state.snowflakes)
        {
            if (!falling)
            {
                continue;
            }

            // Check if this snowflake is at the bottom of the canvas
            // If yes, stop it
            if (y >= CanvasHeight - 1)
            {
                falling = false;
                tallestHeight = CanvasHeight - y;
                state.canvas[x + y * CanvasWidth] = true;
                continue;
            }

            if (getCanvasPixel(state.canvas, x, y + 1) && getCanvasPixel(state.canvas, x - 1, y + 1) && getCanvasPixel(state.canvas, x + 1, y + 1))
            {
                falling = false;
                tallestHeight = CanvasHeight - y;
                state.canvas[x + y * CanvasWidth] = true;
                continue;
            }

            if (x == 0 && getCanvasPixel(state.canvas, 0, y + 1) && getCanvasPixel(state.canvas, 1, y + 1))
            {
                falling = false;
                tallestHeight = CanvasHeight - y;
                state.canvas[y * CanvasWidth] = true;
                continue;
            }

            if (x == CanvasWidth - 1 && getCanvasPixel(state.canvas, CanvasWidth - 1, y + 1) && getCanvasPixel(state.canvas, CanvasWidth - 2, y + 1))
            {
                falling = false;
                tallestHeight = CanvasHeight - y;
                state.canvas[CanvasWidth - 1 + y * CanvasWidth] = true;
                continue;
            }

            // Make the snowflake fall randomly
            y += rand() % 2 + (y < CanvasHeight - 2 ? 1 : 0);

            // Move sideways randomly
            x = std::max(0, std::min(CanvasWidth - 1, x + rand() % 3 - 1));
        }

        // Add new snowflakes
        for (auto& [x, y, falling] : state.snowflakes)
        {
            if (falling)
            {
                continue;
            }

#if 0
            // Simulate that snowflakes are falling from the actual clouds
            // This algorithm tends to create heaps of snow while leaving other parts empty
            const auto& cloud = state.clouds[rand() % state.clouds.size()];
            x = cloud.x + rand() % 25; // TODO use the actual width of the cloud shape
            y = cloud.y + 14 + rand() % 5 - 2; // TODO use the actual height of the cloud shape
#else            
            x = rand() % CanvasWidth;
            y = rand() % 5 + 8;
#endif

            falling = true;
        }

        // Shift down the canvas
        if (tallestHeight > 15)
        {
            for (auto x = 0; x < CanvasWidth; ++x)
            {
                for (auto y = CanvasHeight - 2; y > 0; --y)
                {
                    state.canvas[x + (y + 1) * CanvasWidth] = state.canvas[x + y * CanvasWidth];
                }
            }
        }

        // Move the clouds
        if ((state.ticks & 0b11111) == 0) {
            for (auto& cloud : state.clouds) {
                cloud.x = std::max(0, std::min(static_cast<int>(Display::Width) - 25, cloud.x + rand() % 5 - 2));
                cloud.y = rand() % 3;
            }
        }

        ++state.ticks;
    }
}

//
// Graphics
//

namespace Graphics
{
    using FrameBuffer = std::array<uint8_t, Display::Width * Display::Lines>;

    template <typename ShapeDataType>
    void drawBitmap(FrameBuffer& fb, const unsigned x, const unsigned y, const std::tuple<int, int, ShapeDataType>& shape, const bool mask = false)
    {
        const auto& [width, height, data] = shape;

        const unsigned alignmentOffset = y & 0b111;
        const unsigned startLine = y >> 3;
        const unsigned lineCount = height / 8 + (height % 8 > 0 ? 1 : 0);
        const unsigned extraLines = alignmentOffset > 0 ? 1 : 0;

        for (auto line = 0u; line < lineCount + extraLines && line < Display::Lines; ++line) {
            for (auto col = x; col < x + width && col < Display::Width; ++col) {
                auto& fbByte = fb[col + (startLine + line) * Display::Width];
                
                if (line < lineCount) {
                    const auto b = data[line * width + col - x] << alignmentOffset;
                    if (!mask) {
                        fbByte |= b;
                    } else {
                        fbByte &= b | ~(0xff << alignmentOffset);
                    }
                }

                if (line > 0 && alignmentOffset > 0) {
                    const auto b = data[(line - 1) * width + col - x] >> (8 - alignmentOffset);
                    if (!mask) {
                        fbByte |= b;
                    } else {
                        fbByte &= b | ~(0xff >> (8 - alignmentOffset));
                    }
                }
            }
        }
    }

    void drawScene(Logic::State& state)
    {
        FrameBuffer fb{};

        // Draw the snowflakes
        for (const auto [x, y, falling] : state.snowflakes) {
            if (falling) {
                fb[x + (y >> 3) * Display::Width] |= 1 << (y & 0b111);
            }
        }

        // Draw the clouds
        for (const auto& [x, y, shape] : state.clouds) {
            switch (shape) {
                case Logic::Cloud::Shape::Shape1:
                    drawBitmap(fb, x, y, Logic::Sprites::Cloud1);
                    break;
            }
        }

        // Draw the canvas
        for (auto col = 0u; col < Logic::CanvasWidth; ++col) {
            for (auto row = 0u; row < Logic::CanvasHeight; ++row) {
                if (state.canvas[col + row * Logic::CanvasWidth]) {
                    fb[col + (row >> 3) * Display::Width] |= 1 << (row & 0b111);
                }
            }
        }

        // Draw the snowman
        constexpr auto snowmanX = 20u, snowmanY = Display::Height - 25;
        drawBitmap(fb, snowmanX - 1, snowmanY - 1, Logic::Sprites::Snowman1Mask, true);
        drawBitmap(fb, snowmanX, snowmanY, Logic::Sprites::Snowman1);

        // Send the frame buffer to the display
        for (auto line = 0u; line < Display::Lines; ++line) {
            Display::setColumn(0);
            Display::setLine(line);
            Display::sendData(fb.data() + line * Display::Width, Display::Width);
        }
    }
}

int main()
{
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);

    oled_i2c_init();

    Display::init();
    Display::setContrast(10);

#if 0
    while (getchar() != ' ') {
        sleep_ms(100);
    }
#endif

    printf("Running snowfall\n");

    // gpio_put(PICO_DEFAULT_LED_PIN, 1);

    Logic::State logicState;

    for (auto& [x, y, falling] : logicState.snowflakes)
    {
        x = rand() % Logic::CanvasWidth;
        y = rand() % 50;
        falling = true;
    }

    for (auto i = 0u; i < logicState.clouds.size(); ++i) {
        logicState.clouds[i].x = i * (Display::Width / Logic::MaxClouds);
        logicState.clouds[i].y = rand() % 3;
    }

    while (true) {
        Logic::advance(logicState);
        Graphics::drawScene(logicState);
        sleep_ms(15);
    }

    return 0;
}