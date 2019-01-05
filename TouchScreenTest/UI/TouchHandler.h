#pragma once

#include <cstdint>
#include <memory>

#include "Event.h"
#include "Point.h"

namespace UI
{

class TouchHandler
{
public:
    std::unique_ptr<TouchEvent> handleRawEvent(uint8_t x, uint8_t y, bool pressed);

private:
    bool m_pressed = false;
    Point m_lastPos;
};

}
