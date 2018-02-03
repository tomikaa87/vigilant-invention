#include "ui.h"

#include <string.h>

namespace ui
{

oled_ui::oled_ui(u8g2_t* u8g2)
    : _u8g2{ u8g2 }
{
}

bool oled_ui::add_screen(screen* scr)
{
    if (_screen_cnt >= max_screens)
        return false;

    _screens[_screen_cnt++] = scr;

    // This is the first screen, switch to it automatically
    if (_screen_cnt == 1)
        switch_screen(0);
}

void oled_ui::switch_screen(uint8_t screen_idx)
{
    if (screen_idx >= max_screens ||
        screen_idx >= _screen_cnt ||
        screen_idx == _current_screen_idx)
    {
        return;
    }

    _current_screen_idx = screen_idx;
}

uint8_t oled_ui::screen_count() const
{
    return _screen_cnt;
}

void oled_ui::task()
{
    u8g2_ClearBuffer(_u8g2);

    auto scr = _screens[_current_screen_idx];
    if (scr)
        scr->draw(_u8g2, 0, 0);

    draw_nav_bar(52, scr);

    u8g2_SendBuffer(_u8g2);
}

void oled_ui::draw_nav_bar(uint8_t y, const screen* const scr)
{
    // Top separator
    u8g2_DrawHLine(_u8g2, 0, y, 128);

    u8g2_SetFont(_u8g2, u8g2_font_pxplusibmvga8_tr);

    // "Navigate left (<)" indicator
    if (_current_screen_idx > 0)
        u8g2_DrawStr(_u8g2, 0, y + 12, "<");

    // "Navigate right (>)" indicator
    if (_current_screen_idx < _screen_cnt - 1)
        u8g2_DrawStr(_u8g2, 121, y + 12, ">");

    // Screen name
    if (scr)
    {
        uint8_t name_x = 64 - scr->name().length() * 8 / 2;
        u8g2_DrawStr(_u8g2, name_x, y + 12, scr->name().c_str());
    }
}

}