#pragma once

#include <cstdint>
#include <u8g2.h>

namespace ui
{

class screen
{
public:
    virtual const char* name() const = 0;
    virtual void draw(u8g2_t* u8g2, const uint8_t x, const uint8_t y) = 0;
};

}