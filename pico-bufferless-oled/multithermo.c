#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "drawing.h"
#include "ssd1306.h"

#include "widget.h"

static const uint8_t Bitmap1[20 * 3] = {
    // page 0
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11100000,
    0b11111000,
    0b00011100,
    0b00001110,
    0b11111111,
    0b11110000,
    0b00000000,
    0b00000000,
    0b10000000,
    0b11000000,
    0b11100000,
    0b11100000,
    0b00000000,
    0b00000000,

    // page 1
    0b11110000,
    0b11111100,
    0b00001110,
    0b00111100,
    0b01110000,
    0b01101110,
    0b11111111,
    0b00000001,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000011,
    0b00000110,
    0b00001100,
    0b00011111,
    0b00000011,
    0b00000000,
    0b00001111,
    0b11111111,
    0b11110000,

    // page 2
    0b00000000,
    0b00000111,
    0b00001111,
    0b00011100,
    0b00111000,
    0b01110000,
    0b01100000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b01100000,
    0b01110000,
    0b00111000,
    0b00011100,
    0b00001111,
    0b00000111,
    0b00000000
};

#define ARRAY_LEN(_A) (sizeof(_A) / sizeof(_A[0]))

int main()
{
    w_widget w;
    w_rect wr = { .size.width = 10, .size.height = 10, .pos.x = 0, .pos.y = 10 };
    w_widget_init(&w, &wr);
    wr.pos.y = 0;
    w_widget_set_rect(&w, &wr);

    const w_widget* const widgets[] = { &w };
    w_widget_draw(widgets, ARRAY_LEN(widgets));

    return 0;

    srand(time(NULL));

    printf("Initializing the display...\n");

    ssd1306_init();
    ssd1306_set_brightness(0);
    ssd1306_clear();

    struct w_text txt1;
    w_text_init(&txt1, 10, 10, 30, 10, "TEST");

#if 1

    const struct widget* wst[] = {
        &txt1.widget
    };

    draw_widgets(wst, ARRAY_LEN(wst));

// #else

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
        usleep(1000 * 10);
    }

    return 0;
#endif
}
