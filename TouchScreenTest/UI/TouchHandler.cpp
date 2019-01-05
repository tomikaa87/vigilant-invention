#include "TouchHandler.h"

std::unique_ptr<UI::TouchEvent> UI::TouchHandler::handleRawEvent(uint8_t x, uint8_t y, bool pressed)
{
    Point pos{ x, y };

    if (m_pressed != pressed)
    {
        m_pressed = pressed;

        if (pressed)
            return std::make_unique<UI::TouchPressEvent>(pos);

        return std::make_unique<UI::TouchReleaseEvent>(pos);
    }

    if (pos != m_lastPos)
        return std::make_unique<UI::TouchMoveEvent>(m_lastPos, pos);

    return{};
}
