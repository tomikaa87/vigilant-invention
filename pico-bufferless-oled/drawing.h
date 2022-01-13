#pragma once

#include <stdint.h>

void draw_bmp(
    const uint8_t* bitmap,
    uint8_t width,
    uint8_t x,
    uint8_t page
);

void draw_mp_bmp(
    const uint8_t* mp_bitmap,
    uint8_t width,
    uint8_t page_count,
    uint8_t x,
    uint8_t start_page
);

void draw_mp_bmp_xy(
    const uint8_t* mp_bitmap,
    const uint8_t width,
    const uint8_t page_count,
    const uint8_t x,
    const uint8_t y
);

struct widget {
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
    uint8_t (* draw_fn)(void* arg, uint8_t block, uint8_t page, uint8_t col);
    void* draw_arg;
};

struct w_bitmap {
    struct widget widget;
    const uint8_t* bitmap;
    uint8_t start_page;
    uint8_t end_page;
    uint8_t page_cnt;
    uint8_t blk_offs;
};

void w_bitmap_init(
    struct w_bitmap* wb,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    const uint8_t* bmp
);

void w_bitmap_set_pos(
    struct w_bitmap* wb,
    uint8_t x,
    uint8_t y
);

struct w_text {
    struct widget widget;
    const char* txt;
    uint8_t total_len;
    uint8_t char_blk_len;
    uint8_t start_page;
    uint8_t end_page;
    uint8_t page_cnt;
    uint8_t blk_offs;
};

void w_text_init(
    struct w_text* wt,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    const char* txt
);

void draw_widgets(const struct widget** ws, const uint8_t count);