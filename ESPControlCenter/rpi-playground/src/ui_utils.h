#pragma once

#include <cstdint>
#include <u8g2.h>

namespace ui::utils
{

void draw_progress_bar(u8g2_t* u8g2,
                       const uint8_t x,
                       const uint8_t y,
                       const uint8_t width,
                       const uint8_t height,
                       const uint8_t progress);

}