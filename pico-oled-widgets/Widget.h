#pragma once

#include <string>
#include <vector>

class Painter;

struct Size
{
    int w = 0;
    int h = 0;
};

struct Point
{
    int x = 0;
    int y = 0;

    Point operator+(const Point& p) const
    {
        return Point{ x + p.x, y + p.y };
    }
};

struct Rect
{
    Point pos;
    Size size;

    bool contains(const Point& p) const
    {
        return
            p.x >= pos.x
            && p.x < pos.x + size.w
            && p.y >= pos.y
            && p.y < pos.y + size.h;
    }
};

class Widget
{
    friend class Painter;

public:
    explicit Widget(Widget* parent = nullptr);
    Widget(std::string name, Widget* parent = nullptr);

    virtual ~Widget();

    Widget(const Widget&) = delete;
    Widget(Widget&&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget& operator=(Widget&&) = delete;

    const Point& pos() const { return _rect.pos; }
    const Size& size() const { return _rect.size; }
    const Rect& rect() const { return _rect; }

    void setPos(Point p);
    void setSize(Size s);

    Point mapToGlobal(const Point& p) const;
    Point mapToParent(const Point& p) const;

protected:
    std::string _name;
    Widget* const _parentWidget;
    std::vector<Widget*> _childWidgets;
    Rect _rect;

    // virtual void paintEvent();
};