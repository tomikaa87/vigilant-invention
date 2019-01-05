#pragma once

#include <cstdint>

namespace UI
{

class Size
{
public:
    Size() = default;
    Size(uint8_t w, uint8_t h)
        : m_w{ w }, m_h{ h }
    {}

    inline uint8_t width() const
    {
        return m_w;
    }

    inline uint8_t height() const
    {
        return m_h;
    }

    inline bool operator==(const Size& o) const
    {
        return o.m_w == m_w && o.m_h == m_h;
    }

    inline bool operator!=(const Size& o) const
    {
        return !(o == *this);
    }

private:
    uint8_t m_w = 0;
    uint8_t m_h = 0;
};

}
