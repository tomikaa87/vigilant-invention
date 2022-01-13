#include "drawing.h"
#include "ssd1306.h"

#include <stdio.h>
#include <string.h>

#define PAGES   8
#define COLUMNS 128

#define NORMAL_CHAR_SPACING 1
#define NORMAL_CHAR_WIDTH   5
#define NORMAL_CHAR_HEIGHT  8
#define NORMAL_CHAR_COUNT   59
static const uint8_t normal_charset[NORMAL_CHAR_COUNT][NORMAL_CHAR_WIDTH] = {
    // [space]
    {
        0, 0, 0, 0, 0
    },
    // !
    {
        0b00000000,
        0b00000000,
        0b01001111,
        0b00000000,
        0b00000000
    },
    // "
    {
        0b00000000,
        0b00000111,
        0b00000000,
        0b00000111,
        0b00000000
    },
    // #
    {
        0b00010100,
        0b01111111,
        0b00010100,
        0b01111111,
        0b00010100
    },
    // $
    {
        0b00100100,
        0b00101010,
        0b01111111,
        0b00101010,
        0b00010010
    },
    // %
    {
        0b00100011,
        0b00010011,
        0b00001000,
        0b01100100,
        0b01100010
    },
    // &
    {
        0b00110110,
        0b01001001,
        0b01010101,
        0b00100010,
        0b01010000
    },
    // '
    {
        0b00000000,
        0b00000101,
        0b00000011,
        0b00000000,
        0b00000000
    },
    // (
    {
        0b00000000,
        0b00011100,
        0b00100010,
        0b01000001,
        0b00000000
    },
    // )
    {
        0b00000000,
        0b01000001,
        0b00100010,
        0b00011100,
        0b00000000
    },
    // *
    {
        0b00010100,
        0b00001000,
        0b00111110,
        0b00001000,
        0b00010100
    },
    // +
    {
        0b00001000,
        0b00001000,
        0b00111110,
        0b00001000,
        0b00001000,
    },
    // ,
    {
        0b00000000,
        0b01010000,
        0b00110000,
        0b00000000,
        0b00000000
    },
    // -
    {
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000
    },
    // .
    {
        0b00000000,
        0b01100000,
        0b01100000,
        0b00000000,
        0b00000000
    },
    // /
    {
        0b00100000,
        0b00010000,
        0b00001000,
        0b00000100,
        0b00000010
    },
    // 0
    {
        0b00111110,
        0b01010001,
        0b01001001,
        0b01000101,
        0b00111110
    },
    // 1
    {
        0b00000000,
        0b01000010,
        0b01111111,
        0b01000000,
        0b00000000
    },
    // 2
    {
        0b01000010,
        0b01100001,
        0b01010001,
        0b01001001,
        0b01000110
    },
    // 3
    {
        0b00100001,
        0b01000001,
        0b01000101,
        0b01001011,
        0b00110001
    },
    // 4
    {
        0b00011000,
        0b00010100,
        0b00010010,
        0b01111111,
        0b00010000
    },
    // 5
    {
        0b00100111,
        0b01000101,
        0b01000101,
        0b01000101,
        0b00111001
    },
    // 6
    {
        0b00111100,
        0b01001010,
        0b01001001,
        0b01001001,
        0b00110000
    },
    // 7
    {
        0b00000001,
        0b01110001,
        0b00001001,
        0b00000101,
        0b00000011
    },
    // 8
    {
        0b00110110,
        0b01001001,
        0b01001001,
        0b01001001,
        0b00110110
    },
    // 9
    {
        0b00000110,
        0b01001001,
        0b01001001,
        0b00101001,
        0b00011110
    },
    // :
    {
        0b00000000,
        0b00110110,
        0b00110110,
        0b00000000,
        0b00000000
    },
    // ;
    {
        0b00000000,
        0b01010110,
        0b00110110,
        0b00000000,
        0b00000000
    },
    // <
    {
        0b00001000,
        0b00010100,
        0b00100010,
        0b01000001,
        0b00000000
    },
    // =
    {
        0b00010100,
        0b00010100,
        0b00010100,
        0b00010100,
        0b00010100
    },
    // >
    {
        0b00000000,
        0b01000001,
        0b00100010,
        0b00010100,
        0b00001000
    },
    // ?
    {
        0b00000010,
        0b00000001,
        0b01010001,
        0b00001001,
        0b00000110
    },
    // @
    {
        0b00110010,
        0b01001001,
        0b01111001,
        0b01000001,
        0b00111110
    },
    // A
    {
        0b01111110,
        0b00010001,
        0b00010001,
        0b00010001,
        0b01111110
    },
    // B
    {
        0b01111111,
        0b01001001,
        0b01001001,
        0b01001001,
        0b00110110
    },
    // C
    {
        0b00111110,
        0b01000001,
        0b01000001,
        0b01000001,
        0b00100010
    },
    // D
    {
        0b01111111,
        0b01000001,
        0b01000001,
        0b00100010,
        0b00011100
    },
    // E
    {
        0b01111111,
        0b01001001,
        0b01001001,
        0b01001001,
        0b01000001
    },
    // F
    {
        0b01111111,
        0b00001001,
        0b00001001,
        0b00001001,
        0b00000001
    },
    // G
    {
        0b00111110,
        0b01000001,
        0b01001001,
        0b01001001,
        0b01111010
    },
    // H
    {
        0b01111111,
        0b00001000,
        0b00001000,
        0b00001000,
        0b01111111
    },
    // I
    {
        0b00000000,
        0b01000001,
        0b01111111,
        0b01000001,
        0b00000000
    },
    // J
    {
        0b00100000,
        0b01000000,
        0b01000001,
        0b00111111,
        0b00000001
    },
    // K
    {
        0b01111111,
        0b00001000,
        0b00010100,
        0b00100010,
        0b01000001
    },
    // L
    {
        0b01111111,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000
    },
    // M
    {
        0b01111111,
        0b00000010,
        0b00001100,
        0b00000010,
        0b01111111
    },
    // N
    {
        0b01111111,
        0b00000100,
        0b00001000,
        0b00010000,
        0b01111111
    },
    // O
    {
        0b00111110,
        0b01000001,
        0b01000001,
        0b01000001,
        0b00111110
    },
    // P
    {
        0b01111111,
        0b00001001,
        0b00001001,
        0b00001001,
        0b00000110
    },
    // Q
    {
        0b00111110,
        0b01000001,
        0b01010001,
        0b00100001,
        0b01011110
    },
    // R
    {
        0b01111111,
        0b00001001,
        0b00011001,
        0b00101001,
        0b01000110
    },
    // S
    {
        0b01000110,
        0b01001001,
        0b01001001,
        0b01001001,
        0b00110001
    },
    // T
    {
        0b00000001,
        0b00000001,
        0b01111111,
        0b00000001,
        0b00000001
    },
    // U
    {
        0b00111111,
        0b01000000,
        0b01000000,
        0b01000000,
        0b00111111
    },
    // V
    {
        0b00011111,
        0b00100000,
        0b01000000,
        0b00100000,
        0b00011111
    },
    // W
    {
        0b00111111,
        0b01000000,
        0b00111000,
        0b01000000,
        0b00111111
    },
    // X
    {
        0b01100011,
        0b00010100,
        0b00001000,
        0b00010100,
        0b01100011
    },
    // Y
    {
        0b00000111,
        0b00001000,
        0b01110000,
        0b00001000,
        0b00000111
    },
    // Z
    {
        0b01100001,
        0b01010001,
        0b01001001,
        0b01000101,
        0b01000011
    },
};

static const uint8_t normal_char_placeholder[NORMAL_CHAR_WIDTH] = {
    0b01111111,
    0b01010101,
    0b01001001,
    0b01010101,
    0b01111111
};

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

void draw_bmp(
    const uint8_t* bitmap,
    uint8_t width,
    uint8_t x,
    uint8_t page)
{
    if (page > PAGES || width == 0 || x + width >= COLUMNS)
        return;

    ssd1306_set_page(page);
    ssd1306_set_start_column(x);
    ssd1306_send_data(bitmap, width, 0);
}

void draw_mp_bmp(
    const uint8_t* mp_bitmap,
    uint8_t width,
    uint8_t page_count,
    uint8_t x,
    uint8_t start_page)
{
    if (start_page + page_count > PAGES)
        return;

    const uint8_t* bitmap = mp_bitmap;

    for (uint8_t page = start_page; page < start_page + page_count; ++page) {
        draw_bmp(bitmap, width, x, page);
        bitmap += width;
    }
}

void draw_mp_bmp_xy(
    const uint8_t* mp_bitmap,
    const uint8_t width,
    const uint8_t page_count,
    const uint8_t x,
    const uint8_t y
)
{
    const uint8_t* bitmap = mp_bitmap;

    const uint8_t blockOffset = y & 0b111;
    const uint8_t startPage = y >> 3;

    //printf("startPage=%u, blockOffset=%u\n", startPage, blockOffset);

    ssd1306_page_addressing();

    for (uint8_t page = 0; page < page_count + (blockOffset > 0 ? 1 : 0) && page < PAGES; ++page)
    {
        ssd1306_set_page(startPage + page);
        ssd1306_set_start_column(x);
        // ssd1306_set_column_address(x, x + width);

        for (uint8_t col = x; col < COLUMNS && col < width + x; ++col)
        {
            uint8_t block = 0;

            if (page < page_count)
            {
                const uint8_t blockAddress = page * width + col - x;
                block = bitmap[blockAddress] << blockOffset;
            }

            if (page > 0 && blockOffset > 0)
            {
                const int prevPageBlockAddress = (page - 1) * width + col - x;
                block |= bitmap[prevPageBlockAddress] >> (8 - blockOffset);
            }

            ssd1306_send_data(&block, 1, 0);
        }
    }
}

uint8_t test_widget_draw(
    uint8_t block,
    const uint8_t page,
    const uint8_t col
)
{
    const uint8_t x = 0;
    const uint8_t y = 1;
    const uint8_t w = 20;
    const uint8_t h = 24;
    const uint8_t page_count = 3;
    const uint8_t* bitmap = Bitmap1;

    if (col < x || col >= x + w) {
        return block;
    }

    const uint8_t start_page = y >> 3;
    const uint8_t block_offset = y & 0b111;
    const uint8_t end_page = h / 8 + (block_offset > 0 ? 1 : 0);

    if (page < start_page || page >= end_page) {
        return block;
    }

    const uint8_t rel_page = page - start_page;

    if (rel_page < page_count) {
        const uint8_t block_address = rel_page * w + col - x;
        block |= bitmap[block_address] << block_offset;
    }

    if (rel_page > 0 && block_offset > 0) {
        const int prev_page_block_address = (rel_page - 1) * w + col - x;
        block |= bitmap[prev_page_block_address] >> (8 - block_offset);
    }

    return block;
}

uint8_t test_widget2_draw(
    uint8_t block,
    const uint8_t page,
    const uint8_t col
)
{
    const uint8_t x = 15;
    const uint8_t y = 7;
    const uint8_t w = 20;
    const uint8_t h = 24;
    const uint8_t* bitmap = Bitmap1;

    if (col < x || col >= x + w) {
        return block;
    }

    const uint8_t start_page = y >> 3;
    const uint8_t block_offset = y & 0b111;
    const uint8_t page_count = h / 8 + (h % 8 > 0 ? 1 : 0);
    const uint8_t end_page = page_count + (block_offset > 0 ? 1 : 0);

    if (page < start_page || page >= end_page) {
        return block;
    }

    const uint8_t rel_page = page - start_page;

    if (rel_page < page_count) {
        const uint8_t block_address = rel_page * w + col - x;
        block |= bitmap[block_address] << block_offset;
    }

    if (rel_page > 0 && block_offset > 0) {
        const int prev_page_block_address = (rel_page - 1) * w + col - x;
        block |= bitmap[prev_page_block_address] >> (8 - block_offset);
    }

    return block;
}

void draw_widgets(const struct widget** ws, const uint8_t count)
{
#if 0
    printf("draw_widgets: count=%u\n", count);
#endif

    ssd1306_set_page(0);
    ssd1306_set_start_column(0);

    for (uint8_t page = 0; page < PAGES; ++page) {

        uint8_t chunk[32] = { 0 };

        for (uint8_t col = 0; col < COLUMNS; ++col) {
            if (col > 0 && col % sizeof(chunk) == 0) {
                ssd1306_send_data(chunk, sizeof(chunk), 0);
                memset(chunk, 0, sizeof(chunk));
            }

            uint8_t* block = &chunk[col % sizeof(chunk)];

            for (uint8_t i = 0; i < count; ++i) {
                *block = ws[i]->draw_fn(ws[i]->draw_arg, *block, page, col);
            }
        }

        ssd1306_send_data(chunk, sizeof(chunk), 0);
    }
}

uint8_t _draw_bitmap(void* arg, uint8_t block, uint8_t page, uint8_t col)
{
    struct w_bitmap* wb = (struct w_bitmap*)arg;

    if (col < wb->widget.x
        || col >= wb->widget.x + wb->widget.w
        || page < wb->start_page
        || page >= wb->end_page
    ) {
        return block;
    }

    const uint8_t rel_page = page - wb->start_page;

    if (rel_page < wb->page_cnt) {
        const uint8_t block_address = rel_page * wb->widget.w + col - wb->widget.x;
        block |= wb->bitmap[block_address] << wb->blk_offs;
    }

    if (rel_page > 0 && wb->blk_offs > 0) {
        const int prev_page_block_address = (rel_page - 1) * wb->widget.w + col - wb->widget.x;
        block |= wb->bitmap[prev_page_block_address] >> (8 - wb->blk_offs);
    }

    return block;
}

uint8_t _draw_text(void* arg, uint8_t block, uint8_t page, uint8_t col)
{
    struct w_text* w = (struct w_text*)arg;

    if (w->total_len == 0
        || col >= w->widget.x + w->total_len
        || col < w->widget.x
        || col >= w->widget.x + w->widget.w
        || page < w->start_page
        || page >= w->end_page
    ) {
        return block;
    }

#if 0
    printf("_draw_text: page=%u, col=%u\n", page, col);
#endif

    const uint8_t rel_x = col - w->widget.x;
    const uint8_t char_blk_idx = rel_x / w->char_blk_len;
    const uint8_t char_blk_x = rel_x % w->char_blk_len;
    const uint8_t glyph_idx = w->txt[char_blk_idx] - ' ';
    const uint8_t* glyph = glyph_idx < NORMAL_CHAR_COUNT ? normal_charset[glyph_idx] : normal_char_placeholder;
    const uint8_t rel_page = page - w->start_page;

#if 0
    printf("  rel_x=%u, char_blk_idx=%u, char_blk_x=%u, glyph_idx=%u, glyph=%p, rel_page=%u\n",
        rel_x, char_blk_idx, char_blk_x, glyph_idx, glyph, rel_page
    );
#endif

    if (char_blk_x < NORMAL_CHAR_WIDTH) {
        if (rel_page < w->page_cnt) {
            const uint8_t block_address = rel_page * NORMAL_CHAR_WIDTH + char_blk_x;
#if 0
            printf("  block_address=%u\n", block_address);
#endif
            block |= glyph[block_address] << w->blk_offs;
        }

        if (rel_page > 0 && w->blk_offs > 0) {
            const int prev_page_block_address = (rel_page - 1) * NORMAL_CHAR_WIDTH + char_blk_x;
#if 0
            printf("  pp_block_address=%u\n", prev_page_block_address);
#endif
            block |= glyph[prev_page_block_address] >> (8 - w->blk_offs);
        }
    }

    // // calculate glyph at the given column
    // // draw bitmap block

    // const uint8_t rel_x = col - w->widget.x;

    // const uint8_t gl_width = 5;
    // const uint8_t gl_spacing = 1;

    // const uint8_t ch_idx = rel_x / (gl_width + gl_spacing);
    // const char ch = w->txt[ch_idx];

    // if (ch - ' ' < 0 || ch - ' ' >= sizeof(normal_charset)) {
    //     return block;
    // }

    // const uint8_t* glyph = normal_charset[ch - ' '];
    // const uint8_t gl_x = rel_x % (gl_width + gl_spacing);

    // const uint8_t page_cnt = 1 + (w->blk_offs > 0 ? 1 : 0);

    // printf("  rel_x=%u, ch_idx=%u, ch=%c, gl_x=%u, page_cnt=%u, rel_page=%u, glyph_idx=%u\n",
    //     rel_x, ch_idx, ch, gl_x, page_cnt, rel_page, ch - ' '
    // );

    // if (gl_x < gl_width) {
    //     if (rel_page < page_cnt) {
    //         const uint8_t block_address = rel_page * gl_width + gl_x;
    //         printf("  block_address=%u\n", block_address);
    //         block |= glyph[block_address] << w->blk_offs;
    //     }

    //     if (rel_page > 0 && w->blk_offs > 0) {
    //         const int prev_page_block_address = (rel_page - 1) * gl_width + gl_x;
    //         printf("  pp_block_address=%u\n", prev_page_block_address);
    //         block |= glyph[prev_page_block_address] >> (8 - w->blk_offs);
    //     }
    // } else {
    //     printf("  padding\n");
    // }

    return block;
}

void w_bitmap_init(
    struct w_bitmap* wb,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    const uint8_t* bmp
)
{
    wb->widget.w = w;
    wb->widget.h = h;
    wb->widget.draw_fn = _draw_bitmap;
    wb->widget.draw_arg = (void*)wb;
    wb->bitmap = bmp;

    w_bitmap_set_pos(wb, x, y);

    printf("w_bitmap_init: x=%u, y=%u, w=%u, h=%u, draw_fn=%p, draw_arg=%p, bitmap=%p, start_page=%u, end_page=%u, page_cnt=%u, blk_offs=%u\n",
        wb->widget.x,
        wb->widget.y,
        wb->widget.w,
        wb->widget.h,
        wb->widget.draw_fn,
        wb->widget.draw_arg,
        wb->bitmap,
        wb->start_page,
        wb->end_page,
        wb->page_cnt,
        wb->blk_offs
    );
}

void w_bitmap_set_pos(
    struct w_bitmap* wb,
    uint8_t x,
    uint8_t y
)
{
    wb->widget.x = x;
    wb->widget.y = y;

    wb->start_page = wb->widget.y >> 3;
    wb->blk_offs = wb->widget.y & 0b111;
    wb->page_cnt = wb->widget.h / 8 + (wb->widget.h % 8 > 0 ? 1 : 0);
    wb->end_page = wb->start_page + wb->page_cnt + (wb->blk_offs > 0 ? 1 : 0);

#if 0
    printf("w_bitmap_set_pos: x=%u, y=%u, w=%u, h=%u, start_page=%u, end_page=%u, page_cnt=%u, blk_offs=%u\n",
        wb->widget.x,
        wb->widget.y,
        wb->widget.w,
        wb->widget.h,
        wb->start_page,
        wb->end_page,
        wb->page_cnt,
        wb->blk_offs
    );
#endif
}

void w_text_init(
    struct w_text* wt,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    const char* txt
)
{
    wt->widget.x = x;
    wt->widget.y = y;
    wt->widget.w = w;
    wt->widget.h = h;
    wt->widget.draw_fn = _draw_text;
    wt->widget.draw_arg = (void*)wt;
    wt->txt = txt;

    const int txt_len = strlen(txt);

    if (txt_len > 0) {
        wt->total_len = txt_len * NORMAL_CHAR_WIDTH + (txt_len - 1) * NORMAL_CHAR_SPACING;
    } else {
        wt->total_len = 0;
    }

    wt->char_blk_len = NORMAL_CHAR_WIDTH + NORMAL_CHAR_SPACING;
    wt->start_page = wt->widget.y >> 3;
    wt->blk_offs = wt->widget.y & 0b111;
    wt->page_cnt = NORMAL_CHAR_HEIGHT / 8 + (NORMAL_CHAR_HEIGHT % 8 > 0 ? 1 : 0);
    wt->end_page = wt->start_page + wt->page_cnt + (wt->blk_offs > 0 ? 1 : 0);

    printf("w_text_init: x=%u, y=%u, w=%u, h=%u, draw_fn=%p, draw_arg=%p, txt='%s', page_cnt=%u\n",
        wt->widget.x,
        wt->widget.y,
        wt->widget.w,
        wt->widget.h,
        wt->widget.draw_fn,
        wt->widget.draw_arg,
        wt->txt,
        wt->page_cnt
    );
}