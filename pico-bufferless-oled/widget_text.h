#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "widget.h"

#include <stdint.h>

typedef struct {
    widget_t widget;
    const char* text;
    uint8_t total_len;
    uint8_t char_blk_len;
} widget_text_t;

void widget_text_init(widget_text_t* widget_text, const widget_rect_t* rect);
void widget_text_set_text(widget_text_t* widget_text, const char* text);

#ifdef __cplusplus
} // extern "C"
#endif