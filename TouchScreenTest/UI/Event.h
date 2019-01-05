#pragma once

#include "Point.h"

namespace UI
{

class Event
{
public:
    enum class Type
    {
        Move,
        Press,
        Release
    };

    Event(Type t)
        : type(t)
    {}

    const Type type;
};

class TouchEvent : public Event
{
public:
    TouchEvent(Event::Type type, const Point& pos)
        : Event(type)
        , pos(pos)
    {}

    const Point pos;
};

class TouchMoveEvent : public TouchEvent
{
public:
    TouchMoveEvent(const Point& pos, const Point& oldPos)
        : TouchEvent(Event::Type::Move, pos)
        , oldPos(oldPos)
    {}

    const Point oldPos;
};

class TouchPressEvent : public TouchEvent
{
public:
    TouchPressEvent(const Point& pos)
        : TouchEvent(Event::Type::Press, pos)
    {}
};

class TouchReleaseEvent : public TouchEvent
{
public:
    TouchReleaseEvent(const Point& pos)
        : TouchEvent(Event::Type::Release, pos)
    {}
};

}
