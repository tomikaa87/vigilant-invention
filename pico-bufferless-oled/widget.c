#include "widget.h"

#include "ssd1306.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static widget_extra_t calculate_extra(
    uint8_t widget_y,
    uint8_t widget_h
)
{
    const uint8_t block_offset = widget_y & 0b111;

    widget_extra_t e = {
        .start_page = widget_y >> 3,

        .page_count =
            (widget_h >> 3)
            + ((widget_h & 7) > 0 ? 1 : 0)
            + (block_offset > 0 ? 1 : 0),

        .block_offset = block_offset,
    };

    return e;
}

static void print_extra(const widget_extra_t* const e)
{
    printf("{start_page=%u, page_count=%u, block_offset=%u}",
        e->start_page, e->page_count, e->block_offset
    );
}

static void print_size(const widget_size_t* const s)
{
    printf("{width=%u, height=%u}",
        s->width, s->height
    );
}

static void print_pos(const widget_point_t* const p)
{
    printf("{x=%u, y=%u}",
        p->x, p->y
    );
}

static void print_rect(const widget_rect_t* const r)
{
    printf("{size=");
    print_size(&r->size);
    printf(", pos=");
    print_pos(&r->pos);
    printf("}");
}

static void print_widget(const widget_t* const w)
{
    printf("{rect=");
    print_rect(&w->rect);
    printf(", draw_fn=%p, draw_fn_arg=%p, extra=",
        w->draw_fn, w->draw_fn_arg
    );
    print_extra(&w->extra);
    printf("}");
}

static bool outside_drawing_area(
    const widget_t* const w,
    const uint8_t page,
    const uint8_t col
)
{
    return
        col < w->rect.pos.x
        || col >= w->rect.pos.x + w->rect.size.width
        || page < w->extra.start_page
        || page >= w->extra.start_page + w->extra.page_count;
}

static void draw_widget(
    void* arg,
    uint8_t* block,
    uint8_t page,
    uint8_t col
)
{
    const widget_t* const w = (widget_t*)arg;

    const uint8_t rel_page = page - w->extra.start_page;

    if (rel_page < w->extra.page_count) {
        const uint8_t data = 0xff << w->extra.block_offset;
        *block |= w->extra.inverted ? ~data : data;
    }

    if (rel_page > 0 && w->extra.block_offset > 0) {
        const uint8_t data = 0xff >> (8 - w->extra.block_offset);
        *block |= w->extra.inverted ? ~data : data;
    }
}

void widget_init(widget_t* const widget, const widget_rect_t* rect)
{
    widget->rect = *rect;
    widget->draw_fn = draw_widget;
    widget->draw_fn_arg = widget;
    widget->extra = calculate_extra(rect->pos.y, rect->size.height);
    widget->extra.inverted = false;

#ifdef W_DEBUG_LOG
    printf("w_widget_init: widget=");
    print_widget(widget);
    printf("\n");
#endif
}

void widget_set_rect(widget_t* widget, const widget_rect_t* const rect)
{
    if (rect->pos.y != widget->rect.pos.y
        || rect->size.height != widget->rect.size.height) {
        widget->extra = calculate_extra(
            rect->pos.y,
            rect->size.height
        );
    }

#ifdef W_DEBUG_LOG
    printf("w_widget_set_rect: widget=");
    print_widget(widget);
    printf("\n");
#endif
}

void widget_set_inverted(widget_t* const widget, const bool inverted)
{
    widget->extra.inverted = inverted;
}

void widget_draw(const widget_t* const * const widgets, const uint8_t count)
{
#ifdef W_DEBUG_LOG
    printf("w_widget_draw: count=%u\n", count);
#endif

    ssd1306_set_page(0);
    ssd1306_set_start_column(0);

    for (uint8_t page = 0; page < W_CONFIG_DISP_PAGES; ++page) {

        uint8_t chunk[W_CONFIG_DRAW_CHUNK_SIZE] = { 0 };

        for (uint8_t col = 0; col < W_CONFIG_DISP_COLUMNS; ++col) {
            if (col > 0 && col % sizeof(chunk) == 0) {
                ssd1306_send_data(chunk, sizeof(chunk), 0);
                memset(chunk, 0, sizeof(chunk));
            }

            uint8_t* block = &chunk[col % sizeof(chunk)];

            for (uint8_t i = 0; i < count; ++i) {
                const widget_t* const w = widgets[i];
                if (!w->draw_fn || !w->draw_fn_arg) {
                    continue;
                }

                if (outside_drawing_area(w, page, col)) {
                    continue;
                }

                w->draw_fn(w->draw_fn_arg, block, page, col);
            }
        }

        ssd1306_send_data(chunk, sizeof(chunk), 0);
    }
}