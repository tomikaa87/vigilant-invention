/*
 * UI.c
 *
 *  Created on: 2019. jan. 3.
 *      Author: tomikaa
 */

#include "UI.h"
#include "Widget.h"
#include "Button.h"
#include "Event.h"
#include "TouchHandler.h"

#include <list>

namespace UI
{
    static std::list<Widget*> s_widgets;
    static TouchHandler s_touchHandler;
}

void UI_Init()
{
    static UI::Button btn{ UI::Point{ 0, 0 }, UI::Size{ 100, 40 } };
    static UI::Button btn2{ UI::Point{ 120, 50 }, UI::Size{ 100, 100 } };
    static UI::Button btn3{ UI::Point{ 150, 10 }, UI::Size{ 50, 50 } };

    UI::registerWidget(&btn);
    UI::registerWidget(&btn2);
    UI::registerWidget(&btn3);
}

void UI_SendTouchEvent(uint8_t x, uint8_t y, bool pressed)
{
    auto&& event = UI::s_touchHandler.handleRawEvent(x, y, pressed);

    if (event)
    {
        for (auto widget : UI::s_widgets)
        {
            if (widget->rect().contains(event->pos) || event->type == UI::Event::Type::Release)
                widget->onEvent(*event);
        }
    }
}

void UI_Task()
{
    for (auto widget : UI::s_widgets)
    {
        if (widget->isDirty())
            widget->repaint();
    }
}

void UI::registerWidget(Widget* widget)
{
    s_widgets.push_back(widget);
}

void UI::unregisterWidget(Widget* widget)
{
    s_widgets.remove(widget);
}
