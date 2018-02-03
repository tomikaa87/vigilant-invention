#pragma once

#include "ui.h"

namespace ui::screens
{

class nas_stats : public screen
{
public:
    void draw(u8g2_t* u8g2, const uint8_t x, const uint8_t y) override;
    const char* name() const override;

    uint8_t cpu_usage = 0;
    uint8_t mem_usage = 0;
    uint16_t upload_speed = 0;
    uint16_t download_speed = 0;
};

}