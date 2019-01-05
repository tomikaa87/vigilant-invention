#pragma once

#include "Widget.h"

namespace UI
{

class Button : public Widget
{
public:
    Button(const Point& pos, const Size& size);

    virtual void repaint() override;
    virtual void onEvent(const Event& event) override;

private:
    bool m_pressed = false;
};

}
;
