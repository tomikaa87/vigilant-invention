#pragma once

#include "../screen.h"

#include <string>

namespace ui::screens
{

class air_conditioner : public screen
{
public:
    explicit air_conditioner(const std::string unit_name);

    void draw(u8g2_t* u8g2, const uint8_t x, const uint8_t y);

    uint8_t set_temp = 0;
    uint8_t mode = 0;
    uint8_t vertical_swing = 0;
    uint8_t fan_speed = 0;

private:
    void draw_fan_speed_indicator(u8g2_t* u8g2, const uint8_t x, const uint8_t y, const uint8_t speed);
    void draw_vswing_indicator(u8g2_t* u8g2, const uint8_t x, const uint8_t y, const uint8_t mode);
};

}