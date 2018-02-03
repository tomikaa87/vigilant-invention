#include "ui_utils.h"

#include "../../ui/pxplus-vga8-32.c"

namespace ui::utils
{

void draw_progress_bar(u8g2_t* u8g2,
                       const uint8_t x,
                       const uint8_t y,
                       const uint8_t width,
                       const uint8_t height,
                       const uint8_t progress)
{
    u8g2_DrawFrame(u8g2, x, y, width, height);

    static const uint8_t padding = 2;
    int indicator_width = (width - 2 * padding) * (progress > 100 ? 100 : progress) / 100;

    u8g2_DrawBox(u8g2, x + padding, y + padding, indicator_width, height - 2 * padding);
}

}