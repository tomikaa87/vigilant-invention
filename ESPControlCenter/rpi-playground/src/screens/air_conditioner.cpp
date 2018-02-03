#include "air_conditioner.h"

#include "../ui_utils.h"

namespace ui::screens
{

air_conditioner::air_conditioner(const std::string unit_name)
    : screen{ std::string{ "AC: " } + unit_name }
{
}

void air_conditioner::draw(u8g2_t* u8g2, const uint8_t x, const uint8_t y)
{
    u8g2_SetFont(u8g2, pxplus_vga8_32_nums);
    u8g2_DrawStr(u8g2, x + 49, y + 34, std::to_string(set_temp).c_str());
}

}