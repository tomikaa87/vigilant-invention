#pragma once

#include "screen.h"

#include <u8g2.h>
#include <cstdint>

namespace ui
{

class oled_ui
{
public:
    oled_ui(u8g2_t* u8g2);

    static const uint8_t max_screens = 5;

    bool add_screen(screen* scr);
    void switch_screen(uint8_t screen_idx);

    void task();

private:
    u8g2_t* const _u8g2;

    uint8_t _screen_cnt = 0;
    uint8_t _current_screen_idx = 0;
    screen* _screens[max_screens] = { 0 };

    void draw_nav_bar(const uint8_t y, const screen* const scr);
};

}