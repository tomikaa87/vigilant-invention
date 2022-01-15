#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "Display.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <vector>

#include "Painter.h"
#include "Widget.h"

#include <array>

namespace
{
    volatile bool rotaryEncoder1Low = false;
    volatile bool rotaryEncoder2Low = false;
    volatile bool reLeftDir = false;
    volatile bool reRightDir = false;
    volatile bool reChanged = false;

    enum class RotaryEncoderState {
        Idle,
        FirstPulseA,
        FirstPulseB,
        Turned
    };

    volatile RotaryEncoderState reState = RotaryEncoderState::Idle;

    enum class RotaryEncoderDirection {
        None,
        Left,
        Right
    };

    std::array<RotaryEncoderDirection, 10> reTurns{};
    volatile int reTurnsIn = 0;
    volatile int reTurnsOut = 0;

    int reCounter = 0;
}

void rotaryEncoderIrq(const uint gpio, const uint32_t events)
{
    if ((events & GPIO_IRQ_EDGE_FALL) == GPIO_IRQ_EDGE_FALL) {
        switch (reState) {
            case RotaryEncoderState::Idle:
                if (gpio == 21) {
                    reState = RotaryEncoderState::FirstPulseA;
                } else if (gpio == 20) {
                    reState = RotaryEncoderState::FirstPulseB;
                }
                break;
            
            case RotaryEncoderState::FirstPulseA:
                if (gpio == 20) {
                    reState = RotaryEncoderState::Turned;
                    reTurns[reTurnsIn] = RotaryEncoderDirection::Left;
                    reTurnsIn = (reTurnsIn + 1) % reTurns.size();
                }
                break;

            case RotaryEncoderState::FirstPulseB:
                if (gpio == 21) {
                    reState = RotaryEncoderState::Turned;
                    reTurns[reTurnsIn] = RotaryEncoderDirection::Right;
                    reTurnsIn = (reTurnsIn + 1) % reTurns.size();
                }
                break;
            case RotaryEncoderState::Turned:
                break;
        }
    } else if (gpio_get(20) == 1 && gpio_get(21) == 1) {
        reState = RotaryEncoderState::Idle;
    }
}

int main()
{
    // User LED
    gpio_set_function(25, GPIO_FUNC_SIO);
    gpio_set_dir(25, true);
    gpio_put(25, 0);

    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    stdio_uart_init_full(uart0, 115200, 0, 1);
    printf("UART initialized\r\n");

    printf("Initializing I2C\r\n");
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
    i2c_init(i2c1, 1000000);

    printf("Initializing the display\r\n");
    Display::init();
    Display::setContrast(10);

    Widget textWidget{ "textWidget" };
    textWidget.setPos(Point{ 0, 0 });
    textWidget.setSize(Size{ 128, 64 });
    Painter painter;
    painter.drawText(Rect{ Point{ 2, 2 }, Size{ 126, 10 } }, "Hello, 12345! @Almafa {[456]}");
    painter.drawText(Rect{ Point{ 2, 12 }, Size{ 50, 8 } }, "Some clipped text");
    painter.paint(&textWidget);
    painter.updateDisplay();

#if 0
    printf("Sending some test data to the display\r\n");
    for (auto line = 0u; line < 8; ++line) {
        Display::setColumn(0);
        Display::setLine(line);
        for (auto i = 0u; i < 128u; ++i) {
            Display::sendData(0xAA);
        }
    }

    printf("Testing C++20 vector erase\r\n");
    std::vector<int> v{ 1,2,3,4 };
    printf("  Original vector:");
    for (const auto e : v) {
        printf(" %d", e);
    }
    printf("\r\n  Modified vector:");
    std::erase(v, 3);
    for (const auto e : v) {
        printf(" %d", e);
    }
    printf("\r\n");

    printf("Testing widget painting\r\n");
    Widget wTopLevel{ "TopLevel" }; wTopLevel.setPos(Point{ 10, 10 }); wTopLevel.setSize(Size{ 60, 40 });
    Widget wChild1{ "Child1", &wTopLevel }; wChild1.setPos(Point{ 2, 2 }); wChild1.setSize(Size{ 10, 10 });
    Widget wChild2{ "Child2", &wTopLevel }; wChild2.setPos(Point{ 2, 12 }); wChild2.setSize(Size{ 20, 20 });
    Widget wChild1_1{ "Child1_1", &wChild1 }; wChild1_1.setPos(Point{ 2, 2 }); wChild1_1.setSize(Size{ 3, 3 });
    Widget wChild2_1{ "Child2_1", &wChild2 }; wChild2_1.setPos(Point{ 2, 2 }); wChild2_1.setSize(Size{ 3, 3 });
    Painter painter;
    painter.paint(&wTopLevel);
    // painter.fillRect(Rect{ Point{ 1, 1 }, Size{ 3, 3 } });
    // painter.drawRect(Rect{ Point{ 1, 5 }, Size{ 4, 4 } });
    // painter.drawLine(20, 1, 30, 11);
    // painter.drawEllipse(64, 32, 4, 4);
    // painter.drawEllipse(32, 32, 6, 4);
    painter.drawText(Rect{ Point{ 0, 50 }, Size{} }, "Hello");
    painter.updateDisplay();

    printf("Starting main loop\r\n");
    gpio_put(25, 1);

    gpio_set_irq_enabled_with_callback(
        20,
        GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
        true, 
        rotaryEncoderIrq
    );

    gpio_set_irq_enabled_with_callback(
        21,
        GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
        true, 
        rotaryEncoderIrq
    );
#endif

#if 0
    auto bitShift = 0u;
    const std::vector<uint8_t> lineData(128, 0x01u);
#endif

    while (true) {
        // sleep_ms(500);

        // switch (reDir) {
        //     case RotaryEncoderDirection::Left:
        //         printf("Rotary encoder: <--\r\n");
        //         break;
        //     case RotaryEncoderDirection::Right:
        //         printf("Rotary encoder: -->\r\n");
        //         break;
        //     default:
        //         break;
        // }
        
        // reDir = RotaryEncoderDirection::None;

#if 0
        while (reTurnsOut != reTurnsIn) {
            const auto dir = reTurns[reTurnsOut];
            if (dir != RotaryEncoderDirection::None) {
                printf("Rotary encoder: %s\r\n", dir == RotaryEncoderDirection::Left ? "<--" : "-->");
                reCounter +=
                    dir == RotaryEncoderDirection::Left ? -10 : 10;
                reCounter = std::max(0, std::min(124, reCounter));
                painter.clearBuffer();
                painter.drawRect(Rect{ Point{ 0, 0 }, Size{ 128, 64 } });
                painter.fillRect(Rect{ Point{ 2, 2 }, Size{ reCounter, 60 } });
                painter.updateDisplay();
            }
            reTurnsOut = (reTurnsOut + 1) % reTurns.size();
        }
#endif

        // tight_loop_contents();

#if 0
        if (++bitShift > 7) {
            bitShift = 0;
        }

        printf("Testing the display: bitShift=%u\r\n", bitShift);

        for (auto line = 0u; line < 8; ++line) {
            Display::setColumn(0);
            Display::setLine(line);
            Display::sendData(lineData.data(), lineData.size(), bitShift);
        }
#endif
    }   

    return 0;
}

#if 0

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
        sleep_ms(15)
    }

    return 0;
}

#endif