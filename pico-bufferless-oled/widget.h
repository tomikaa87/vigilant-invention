#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define W_CONFIG_DISP_COLUMNS           (128)
#define W_CONFIG_DISP_PAGES             (8)
#define W_CONFIG_DRAW_CHUNK_SIZE        (32)

#define W_DEBUG_LOG

typedef struct {
    uint8_t width;
    uint8_t height;
} widget_size_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} widget_point_t;

typedef struct {
    widget_size_t size;
    widget_point_t pos;
} widget_rect_t;

typedef void (* widget_draw_fn_t)(void* arg, uint8_t* block, uint8_t page, uint8_t col);

typedef struct {
    uint8_t start_page;
    uint8_t page_count;
    uint8_t block_offset;
    bool inverted;
} widget_extra_t;

typedef struct {
    widget_rect_t rect;
    widget_draw_fn_t draw_fn;
    void* draw_fn_arg;
    widget_extra_t extra;
} widget_t;

void widget_init(widget_t* widget, const widget_rect_t* rect);
void widget_set_rect(widget_t* widget, const widget_rect_t* rect);
void widget_set_inverted(widget_t* widget, bool inverted);
void widget_draw(const widget_t* const * widgets, uint8_t count);

#ifdef __cplusplus
} // extern "C"
#endif