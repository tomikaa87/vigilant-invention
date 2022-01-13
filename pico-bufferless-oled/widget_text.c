#include "widget_text.h"

#include "widget_font.h"

#include <stdio.h>
#include <string.h>

#define WIDGET_TEXT_DEBUG 0

static void draw_widget(
    void* arg,
    uint8_t* block,
    uint8_t page,
    uint8_t col
)
{
    const widget_text_t* const w = (widget_text_t*)arg;

    if (w->total_len == 0
        || col >= w->widget.rect.pos.x + w->total_len
    ) {
        return;
    }

#if WIDGET_TEXT_DEBUG
    printf("text::draw_widget: page=%u, col=%u\n", page, col);
#endif

    const uint8_t rel_x = col - w->widget.rect.pos.x;
    const uint8_t char_blk_idx = rel_x / w->char_blk_len;
    const uint8_t char_blk_x = rel_x % w->char_blk_len;
    const uint8_t glyph_idx = w->text[char_blk_idx] - ' ';
    const uint8_t* glyph_data =
        glyph_idx < WIDGET_DEFAULT_FONT_CHAR_COUNT
            ? widget_default_font[glyph_idx]
            : widget_default_char_placeholder;
    const uint8_t rel_page = page - w->widget.extra.start_page;
    const uint8_t page_count =
        WIDGET_DEFAULT_FONT_CHAR_HEIGHT / 8
        + (WIDGET_DEFAULT_FONT_CHAR_HEIGHT % 8 > 0 ? 1 : 0);

#if WIDGET_TEXT_DEBUG
    printf("  rel_x=%u, char_blk_idx=%u, char_blk_x=%u, glyph_idx=%u, glyph_data=%p, rel_page=%u, c=%c\n",
        rel_x, char_blk_idx, char_blk_x, glyph_idx, glyph_data, rel_page, w->text[char_blk_idx]
    );
#endif

    if (char_blk_x < WIDGET_DEFAULT_FONT_CHAR_WIDTH) {
        if (rel_page < page_count) {
            const uint8_t block_address = rel_page * WIDGET_DEFAULT_FONT_CHAR_WIDTH + char_blk_x;
#if WIDGET_TEXT_DEBUG
            printf("  block_address=%u\n", block_address);
#endif
            const uint8_t data = glyph_data[block_address];
            *block |= (w->widget.extra.inverted ? ~data : data) << w->widget.extra.block_offset;
        }

        if (rel_page > 0 && w->widget.extra.block_offset > 0) {
            const int prev_page_block_address = (rel_page - 1) * WIDGET_DEFAULT_FONT_CHAR_WIDTH + char_blk_x;
#if WIDGET_TEXT_DEBUG
            printf("  pp_block_address=%u\n", prev_page_block_address);
#endif
            const uint8_t data = glyph_data[prev_page_block_address];
            const uint8_t offset = (8 - w->widget.extra.block_offset);
            *block |= ((w->widget.extra.inverted ? ~data : data) >> offset) & (0xff >> offset);
        }
    } else if (w->widget.extra.inverted) {
        // In inverted mode, spacing must be filled
        if (rel_page < page_count) {
            *block |= 0xff << w->widget.extra.block_offset;
        }

        if (rel_page > 0 && w->widget.extra.block_offset > 0) {
            *block |= 0xff >> (8 - w->widget.extra.block_offset);
        }
    }
}

void widget_text_init(widget_text_t* widget_text, const widget_rect_t* rect)
{
    widget_init(&widget_text->widget, rect);
    widget_text->widget.draw_fn = draw_widget;
    widget_text->widget.draw_fn_arg = widget_text;

    widget_text_set_text(widget_text, "");
}

void widget_text_set_text(widget_text_t* widget_text, const char* text)
{
    widget_text->text = text;

    const int length = strlen(text);

    if (length > 0) {
        widget_text->total_len =
            length 
            * WIDGET_DEFAULT_FONT_CHAR_WIDTH
            + (length - 1) 
            * 1 /* spacing */;
    } else {
        widget_text->total_len = 0;
    }

    widget_text->char_blk_len =
        WIDGET_DEFAULT_FONT_CHAR_WIDTH + 1 /* spacing */;
}