#include "Widget.h"

#include <algorithm>

Widget::Widget(Widget* parent)
    : _parentWidget{ parent }
{
    if (_parentWidget) {
        _parentWidget->_childWidgets.push_back(this);
    }
}

Widget::Widget(std::string name, Widget* parent)
    : Widget{ parent }
{
    _name = std::move(name);
}

Widget::~Widget()
{
    if (_parentWidget) {
        std::erase(
            _childWidgets,
            this
        );
    }
}

void Widget::setPos(Point p)
{
    _rect.pos = std::move(p);
}

void Widget::setSize(Size s)
{
    _rect.size = std::move(s);
}

Point Widget::mapToGlobal(const Point& p) const
{
    Point mappedPoint = p;
    const Widget* w = this;

    while (w) {
        mappedPoint = w->mapToParent(mappedPoint);
        w = w->_parentWidget;
    }

    return mappedPoint;
}

Point Widget::mapToParent(const Point& p) const
{
    if (!_parentWidget) {
        return p;
    }

    return Point{
        p.x + _parentWidget->pos().x,
        p.y + _parentWidget->pos().y
    };
}