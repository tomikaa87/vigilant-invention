#pragma once

#include <cstdint>

namespace UI
{

class Point
{
public:
    Point() = default;
    Point(uint8_t x, uint8_t y)
        : m_x{ x }, m_y{ y }
    {}

    inline uint8_t x() const
    {
        return m_x;
    }

    inline uint8_t y() const
    {
        return m_y;
    }

    inline bool operator==(const Point& o) const
    {
        return o.m_x == m_x && o.m_y == m_y;
    }

    inline bool operator!=(const Point& o) const
    {
        return !(o == *this);
    }

private:
    uint8_t m_x = 0;
    uint8_t m_y = 0;
};

}
