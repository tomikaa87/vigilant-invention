#pragma once

#include <cstdint>
#include <string>
#include <u8g2.h>

namespace ui
{

class screen
{
public:
    explicit screen(const std::string name)
        : _name{ name }
    {}

    virtual ~screen() = default;

    inline std::string name() const
    {
        return _name;
    }

    virtual void draw(u8g2_t* u8g2, const uint8_t x, const uint8_t y) = 0;

private:
    const std::string _name;
};

}