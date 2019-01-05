/*
 * Widget.h
 *
 *  Created on: 2019. jan. 3.
 *      Author: tomikaa
 */

#ifndef WIDGET_H_
#define WIDGET_H_

#include "Event.h"
#include "Point.h"
#include "Rect.h"
#include "Size.h"

namespace UI
{

class Widget
{
public:
    Widget();
    Widget(const Point& pos, const Size& size);

    virtual ~Widget();

    bool isDirty() const;
    void update();

    void setVisible(bool visible);
    bool isVisible() const;

    Size size() const;
    void setSize(const Size& size);

    inline const Rect& rect() const
    {
        return m_rect;
    }

    virtual void repaint();
    virtual void onEvent(const Event& event);

protected:
    bool m_dirty = true;
    bool m_visible = true;
    Rect m_rect;
};

} /* namespace UI */

#endif /* WIDGET_H_ */
