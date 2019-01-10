#pragma once

#include <cstdint>

#include "Point.h"
#include "Size.h"

namespace UI
{

class Rect
{
public:
    Rect() = default;

    Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
        : m_topLeft(x, y)
        , m_bottomRight(x + w, y + h)
    {}

    Rect(const Point& topLeft, const Point& bottomRight)
        : m_topLeft(topLeft)
        , m_bottomRight(bottomRight)
    {}

    Rect(const Point& topLeft, const Size& size)
        : m_topLeft(topLeft)
        , m_bottomRight(topLeft.x() + size.width(),
                        topLeft.y() + size.height())
    {}

    inline uint8_t top() const
    {
        return m_topLeft.y();
    }

    inline uint8_t left() const
    {
        return m_topLeft.x();
    }

    inline uint8_t bottom() const
    {
        return m_bottomRight.y();
    }

    inline uint8_t right() const
    {
        return m_bottomRight.x();
    }

    inline uint8_t width() const
    {
        return m_bottomRight.x() - m_topLeft.x();
    }

    inline uint8_t height() const
    {
        return m_bottomRight.y() - m_topLeft.y();
    }

    inline bool contains(const Point& p) const
    {
        return p.x() >= m_topLeft.x()
            && p.x() <= m_bottomRight.x()
            && p.y() >= m_topLeft.y()
            && p.y() <= m_bottomRight.y();
    }

    inline Size size() const
    {
        return Size(width(), height());
    }

    inline void setSize(const Size& size)
    {
        m_bottomRight = Point(m_topLeft.x() + size.width(),
                              m_topLeft.y() + size.height());
    }

private:
    Point m_topLeft;
    Point m_bottomRight;
};

}
