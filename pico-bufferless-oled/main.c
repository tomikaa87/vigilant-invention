// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wpedantic"
// #pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
// #pragma GCC diagnostic ignored "-Wunused-parameter"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/address_mapped.h"
#include "hardware/claim.h"
#include "hardware/clocks.h"
#include "hardware/divider.h"
#include "hardware/dma.h"
#include "hardware/flash.h"
#include "hardware/interp.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/pio_instructions.h"
#include "hardware/pll.h"
#include "hardware/pwm.h"
#include "hardware/resets.h"
#include "hardware/rtc.h"
#include "hardware/spi.h"
#include "hardware/sync.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "hardware/vreg.h"
#include "hardware/watchdog.h"
#include "hardware/xosc.h"
// #pragma GCC diagnostic pop

#include "ssd1306.h"
#include "widget.h"
#include "widget_text.h"

#include <stdio.h>

#define ARRAY_LEN(_A) (sizeof(_A) / sizeof(_A[0]))

extern const uint8_t Bitmap1[20 * 3];

enum
{
    I2CSDA = 26,
    I2CSCL = 27
};

void oled_i2c_init()
{
    gpio_init(I2CSDA);
    gpio_set_function(I2CSDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2CSDA);

    gpio_init(I2CSCL);
    gpio_set_function(I2CSCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2CSCL);

    i2c_init(i2c1, 100 * 1000);
}

void oled_i2c_send(const uint8_t* data, const size_t length)
{
    i2c_write_blocking(i2c1, SSD1306_I2C_ADDRESS, data, length, false);
}

void widget_test_1()
{
    widget_t w;
    widget_rect_t wr = { .size.width = 10, .size.height = 10, .pos.x = 0, .pos.y = 10 };
    widget_init(&w, &wr);
    wr.pos.y = 0;
    widget_set_rect(&w, &wr);

    const widget_t* const widgets[] = { &w };
    widget_draw(widgets, ARRAY_LEN(widgets));
}

void widget_text_test_1()
{
    widget_text_t wt;
    widget_rect_t wr = { .size.width = 30, .size.height = 8, .pos.x = 4, .pos.y = 4 };
    widget_text_init(&wt, &wr);
    widget_text_set_text(&wt, "Test");
    widget_set_inverted(&wt.widget, true);

    const widget_t* const widgets[] = { &wt.widget };
    widget_draw(widgets, ARRAY_LEN(widgets));
}

#if 0
void widget_test_2()
{
    struct w_bitmap wbs[15];
    for (int i = 0; i < ARRAY_LEN(wbs); ++i) {
        w_bitmap_init(&wbs[i], 0, 0, 20, 24, Bitmap1);
    }

    const struct widget* ws[ARRAY_LEN(wbs) + 1];
    for (int i = 0; i < ARRAY_LEN(wbs); ++i) {
        ws[i] = &wbs[i].widget;
    }
    ws[ARRAY_LEN(wbs)] = &txt1.widget;

    uint8_t x[ARRAY_LEN(wbs)], dx[ARRAY_LEN(wbs)];
    uint8_t y[ARRAY_LEN(wbs)], dy[ARRAY_LEN(wbs)];

    for (int i = 0; i < ARRAY_LEN(wbs); ++i) {
        int rx = rand() % (127 - 20) + 1;
        int ry = rand() % (63 - 24) + 1;
        x[i] = rx;
        y[i] = ry;
        dx[i] = rand() % 2 > 0 ? 1 : -1;
        dy[i] = rand() % 2 > 0 ? 1 : -1;
    }

    while (1) {
        for (int i = 0; i < ARRAY_LEN(wbs); ++i) {
            w_bitmap_set_pos(
                &wbs[i],
                x[i],
                y[i]
            );

            x[i] += dx[i];
            y[i] += dy[i];

            if (x[i] >= 128 - wbs[i].widget.w) {
                dx[i] = -1;
            } else if (x[i] == 0) {
                dx[i] = 1;
            }

            if (y[i] >= 64 - wbs[i].widget.h) {
                dy[i] = -1;
            } else if (y[i] == 0) {
                dy[i] = 1;
            }
        }

        draw_widgets(ws, ARRAY_LEN(ws));
        sleep_ms(1);
    }
}
#endif

int main()
{
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);

    oled_i2c_init();
    ssd1306_set_i2c_send_fn(oled_i2c_send);

    ssd1306_init();
    ssd1306_set_brightness(5);

#if 0
    while (getchar() != ' ') {
        sleep_ms(100);
    }
#endif

    printf("Running the test\n");

    // widget_test_1();
    // widget_test_2();
    widget_text_test_1();

    gpio_put(PICO_DEFAULT_LED_PIN, 1);

    printf("Sleeping\n");

    while (true) {
        sleep_ms(1000);
    }

    return 0;
}