#include "air_conditioner.h"

#include "../ui_utils.h"

#define fan_speed_icon_width 11
#define fan_speed_icon_height 11
static unsigned char fan_speed_icon_bits[] = {
    0x20, 0x00, 0x50, 0x00, 0x50, 0x00, 0x30, 0x00, 0xf6, 0x03, 0xd9, 0x04,
    0x7e, 0x03, 0x60, 0x00, 0x50, 0x00, 0x50, 0x00, 0x20, 0x00 };

#define vswing_base_icon_width 6
#define vswing_base_icon_height 12
static unsigned char vswing_base_icon_bits[] = {
    0x0f, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x10, 0x08, 0x07 };

#define vswing_pos0_width 5
#define vswing_pos0_height 1
static unsigned char vswing_pos0_bits[] = {
    0x1f };

#define vswing_pos1_width 6
#define vswing_pos1_height 3
static unsigned char vswing_pos1_bits[] = {
    0x03, 0x0c, 0x30 };

#define vswing_pos2_width 5
#define vswing_pos2_height 5
static unsigned char vswing_pos2_bits[] = {
    0x01, 0x02, 0x04, 0x08, 0x10 };

#define vswing_pos3_width 3
#define vswing_pos3_height 6
static unsigned char vswing_pos3_bits[] = {
    0x01, 0x01, 0x02, 0x02, 0x04, 0x04 };

#define vswing_pos4_width 1
#define vswing_pos4_height 6
static unsigned char vswing_pos4_bits[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };


namespace ui::screens
{

air_conditioner::air_conditioner(const std::string unit_name)
    : screen{ std::string{ "AC: " } + unit_name }
{
}

void air_conditioner::draw(u8g2_t* u8g2, const uint8_t x, const uint8_t y)
{
    // Temperature readout
    u8g2_SetFont(u8g2, pxplus_vga8_32_nums);
    u8g2_DrawStr(u8g2, x + 49, y + 34, std::to_string(set_temp).c_str());

    // Fan speed indicator
    draw_fan_speed_indicator(u8g2, x + 84, 0, fan_speed);

    // Vertical swing indicator
    draw_vswing_indicator(u8g2, x + 98, y + 30, vertical_swing);
}

void air_conditioner::draw_fan_speed_indicator(u8g2_t* u8g2, const uint8_t x, const uint8_t y, const uint8_t speed)
{
    u8g2_DrawXBM(u8g2, x, y, fan_speed_icon_width, fan_speed_icon_height, fan_speed_icon_bits);

    // Indicate automatic fan speed
    if (speed == 0)
    {
        u8g2_SetFont(u8g2, u8g2_font_pxplusibmvga8_tr);
        u8g2_DrawStr(u8g2, x + 13, y + 10, "AUTO");

        return;
    }

    // Draw the first bar
    if (speed > 0)
        u8g2_DrawBox(u8g2, x + 15, y + 6, 7, 2);

    // Draw the second bar
    if (speed > 2)
        u8g2_DrawBox(u8g2, x + 24, y + 4, 7, 4);

    // Draw the third bar
    if (speed > 4)
        u8g2_DrawBox(u8g2, x + 33, y + 2, 7, 6);
}

void air_conditioner::draw_vswing_indicator(u8g2_t* u8g2, const uint8_t x, const uint8_t y, const uint8_t mode)
{
    u8g2_DrawXBM(u8g2, x, y, vswing_base_icon_width, vswing_base_icon_height, vswing_base_icon_bits);

    u8g2_DrawXBM(u8g2, x + 8, y + 9, vswing_pos0_width, vswing_pos0_height, vswing_pos0_bits);
    u8g2_DrawXBM(u8g2, x + 7, y + 11, vswing_pos1_width, vswing_pos1_height, vswing_pos1_bits);
    u8g2_DrawXBM(u8g2, x + 6, y + 13, vswing_pos2_width, vswing_pos2_height, vswing_pos2_bits);
    u8g2_DrawXBM(u8g2, x + 4, y + 14, vswing_pos3_width, vswing_pos3_height, vswing_pos3_bits);
    u8g2_DrawXBM(u8g2, x + 2, y + 14, vswing_pos4_width, vswing_pos4_height, vswing_pos4_bits);
}

}