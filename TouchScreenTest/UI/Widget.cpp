/*
 * Widget.cpp
 *
 *  Created on: 2019. jan. 3.
 *      Author: tomikaa
 */

#include "Widget.h"

UI::Widget::Widget()
{
    // TODO Auto-generated constructor stub
}

UI::Widget::Widget(const Point& pos, const Size& size)
    : m_rect(pos, size)
{
}

UI::Widget::~Widget()
{
    // TODO Auto-generated destructor stub
}

bool UI::Widget::isDirty() const
{
    return m_dirty;
}

void UI::Widget::update()
{
    m_dirty = true;
}

void UI::Widget::setVisible(bool visible)
{
    m_visible = visible;
}

bool UI::Widget::isVisible() const
{
    return m_visible;
}

void UI::Widget::repaint()
{
    m_dirty = false;
}

void UI::Widget::onEvent(const Event& event)
{
}

UI::Size UI::Widget::size() const
{
    return m_rect.size();
}

void UI::Widget::setSize(const Size& size)
{
    if (size != m_rect.size())
    {
        m_rect.setSize(size);
        m_dirty = true;
    }
}
