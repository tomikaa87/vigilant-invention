#include "Button.h"
#include "graphics.h"

UI::Button::Button(const Point& pos, const Size& size)
    : Widget{ pos, size }
{
}

void UI::Button::repaint()
{
    Graphics_DrawRect(m_rect.left(), m_rect.top(), m_rect.right(), m_rect.bottom(), GRAPHICS_COLOR_BLACK);
    Graphics_FillRect(m_rect.left() + 1, m_rect.top() + 1, m_rect.right() - 1, m_rect.bottom() - 1, m_pressed ? GRAPHICS_COLOR_DARKGRAY : GRAPHICS_COLOR_LIGHTGRAY);
    Widget::repaint();
}

void UI::Button::onEvent(const Event& event)
{
    switch (event.type)
    {
    case Event::Type::Press:
        m_pressed = true;
        update();
        break;

    case Event::Type::Release:
        m_pressed = false;
        update();
        break;

    default:
        break;
    }
}
