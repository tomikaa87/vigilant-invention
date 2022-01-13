#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum
{
    WIDGET_DEFAULT_FONT_CHAR_COUNT = 95,
    WIDGET_DEFAULT_FONT_CHAR_WIDTH = 5,
    WIDGET_DEFAULT_FONT_CHAR_HEIGHT = 8
};

extern const uint8_t widget_default_font[WIDGET_DEFAULT_FONT_CHAR_COUNT][WIDGET_DEFAULT_FONT_CHAR_WIDTH];
extern const uint8_t widget_default_char_placeholder[WIDGET_DEFAULT_FONT_CHAR_WIDTH];

#ifdef __cplusplus
} // extern "C"
#endif