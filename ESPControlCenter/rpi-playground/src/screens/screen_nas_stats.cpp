#include "screen_nas_stats.h"
#include "ui_utils.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <u8g2.h>

namespace ui::screens
{

void nas_stats::draw(u8g2_t* u8g2, const uint8_t x, const uint8_t y)
{
    u8g2_SetFont(u8g2, u8g2_font_pxplusibmvga8_tr);

    std::stringstream ss;

    // CPU
    ss << "CPU: " << std::setw(3) << std::setfill(' ') << static_cast<int>(cpu_usage) << "%";
    u8g2_DrawStr(u8g2, x, y + 10, ss.str().c_str());
    ui::utils::draw_progress_bar(u8g2, x + 78, y, 50, 10, cpu_usage);

    // MEM
    ss.str({});
    ss << "MEM: " << std::setw(3) << std::setfill(' ') << static_cast<int>(mem_usage) << "%";
    u8g2_DrawStr(u8g2, x, y + 23, ss.str().c_str());
    ui::utils::draw_progress_bar(u8g2, x + 78, y + 13, 50, 10, mem_usage);

    // UPL
    ss.str({});
    ss << "UPL: " << std::setw(4) << std::setfill(' ') << static_cast<int>(upload_speed) << " K/s";
    u8g2_DrawStr(u8g2, x, y + 36, ss.str().c_str());

    // DNL
    ss.str({});
    ss << "DNL: " << std::setw(4) << std::setfill(' ') << static_cast<int>(download_speed) << " K/s";
    u8g2_DrawStr(u8g2, x, y + 49, ss.str().c_str());
}

const char* nas_stats::name() const
{
    return "NAS STATS";
}

}