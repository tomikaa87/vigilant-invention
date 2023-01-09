#include "Painter.h"
#include "Widget.h"

#include <cmath>
#include <cstdio>

namespace Fonts::Default
{
    constexpr uint8_t Attributes[] = {
        0x02, 0x02, 0x04, 0x06, 0x06, 0x08, 0x05, 0x02,
        0x03, 0x03, 0x06, 0x06, 0x03, 0x05, 0x02, 0x06,
        0x05, 0x03, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
        0x05, 0x05, 0x03, 0x03, 0x04, 0x04, 0x04, 0x05,
        0x09, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
        0x05, 0x02, 0x04, 0x05, 0x04, 0x06, 0x06, 0x05,
        0x05, 0x05, 0x05, 0x05, 0x06, 0x05, 0x06, 0x06,
        0x05, 0x06, 0x05, 0x03, 0x06, 0x03, 0x04, 0x04,
        0x03, 0x05, 0x05, 0x05, 0x05, 0x05, 0x04, 0x05,
        0x05, 0x02, 0x03, 0x05, 0x02, 0x08, 0x05, 0x05,
        0x05, 0x05, 0x04, 0x05, 0x04, 0x05, 0x06, 0x06,
        0x05, 0x05, 0x04, 0x04, 0x02, 0x04, 0x05, 
        0x60,   // BytesPerLine
        0x0A,   // BitmapHeight
        0x01,   // WideCharCount
        0x20    // First WideCharIndex
    };

    constexpr uint8_t Data[] = {
        0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
        0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0xC0, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x20, 0x80, 0x80, 0x00,
        0x00, 0x80, 0xA0, 0x50, 0x78, 0x44, 0x60, 0x80,
        0x80, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
        0x60, 0xC0, 0xE0, 0xE0, 0x20, 0xF0, 0x60, 0xF0,
        0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
        0x42, 0x00, 0x60, 0xE0, 0x70, 0xE0, 0xF0, 0xF0,
        0x70, 0x90, 0x80, 0x20, 0x90, 0x80, 0x88, 0x88,
        0x60, 0xE0, 0x60, 0xE0, 0x70, 0xF8, 0x90, 0x88,
        0xA8, 0x90, 0x88, 0xF0, 0x80, 0x80, 0x40, 0x40,
        0x00, 0x80, 0x00, 0x80, 0x00, 0x10, 0x00, 0x20,
        0x00, 0x80, 0x80, 0x40, 0x80, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x40, 0x80, 0x40, 0x50,
        0x00, 0x80, 0xA0, 0x50, 0xA0, 0xA8, 0x80, 0x80,
        0x80, 0x40, 0x20, 0x20, 0x00, 0x00, 0x00, 0x10,
        0x90, 0x40, 0x10, 0x10, 0x60, 0x80, 0x80, 0x10,
        0x90, 0x90, 0x00, 0x00, 0x20, 0x00, 0x80, 0x10,
        0x99, 0x00, 0x90, 0x90, 0x80, 0x90, 0x80, 0x80,
        0x80, 0x90, 0x80, 0x20, 0xA0, 0x80, 0xD8, 0xC8,
        0x90, 0x90, 0x90, 0x90, 0x80, 0x20, 0x90, 0x88,
        0xA8, 0x90, 0x50, 0x10, 0x80, 0x40, 0x40, 0xA0,
        0x00, 0x40, 0x00, 0x80, 0x00, 0x10, 0x00, 0x40,
        0x00, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x40, 0x80, 0x40, 0xA0,
        0x00, 0x80, 0x00, 0xF8, 0xA0, 0x48, 0x80, 0x00,
        0x80, 0x40, 0xA8, 0x20, 0x00, 0x00, 0x00, 0x10,
        0x90, 0x40, 0x10, 0x10, 0xA0, 0x80, 0x80, 0x20,
        0x90, 0x90, 0x80, 0x40, 0x40, 0xE0, 0x40, 0x10,
        0x85, 0x00, 0x90, 0x90, 0x80, 0x90, 0x80, 0x80,
        0x80, 0x90, 0x80, 0x20, 0xA0, 0x80, 0xA8, 0xA8,
        0x90, 0x90, 0x90, 0x90, 0x80, 0x20, 0x90, 0x50,
        0xA8, 0x90, 0x50, 0x20, 0x80, 0x40, 0x40, 0x00,
        0x00, 0x00, 0x60, 0xE0, 0x70, 0x70, 0x60, 0xE0,
        0x70, 0xE0, 0x80, 0x40, 0x90, 0x80, 0xEC, 0xE0,
        0x60, 0xE0, 0x70, 0xE0, 0x70, 0xE0, 0x90, 0x88,
        0xA8, 0x90, 0x90, 0xE0, 0x40, 0x80, 0x40, 0x00,
        0x00, 0x80, 0x00, 0x50, 0x70, 0x10, 0x50, 0x00,
        0x80, 0x40, 0x70, 0xF8, 0x00, 0x00, 0x00, 0x20,
        0x90, 0x40, 0x60, 0x60, 0xA0, 0xE0, 0xE0, 0x20,
        0x60, 0x70, 0x00, 0x00, 0x80, 0x00, 0x20, 0x20,
        0x9D, 0x00, 0xF0, 0xE0, 0x80, 0x90, 0xE0, 0xE0,
        0xB0, 0xF0, 0x80, 0x20, 0xC0, 0x80, 0x88, 0xA8,
        0x90, 0xE0, 0x90, 0xE0, 0x60, 0x20, 0x90, 0x50,
        0xA8, 0x60, 0x20, 0x40, 0x80, 0x20, 0x40, 0x00,
        0x00, 0x00, 0x10, 0x90, 0x80, 0x90, 0x90, 0x40,
        0x90, 0x90, 0x80, 0x40, 0xA0, 0x80, 0x92, 0x90,
        0x90, 0x90, 0x90, 0x80, 0x80, 0x40, 0x90, 0x88,
        0xA8, 0x90, 0x90, 0x20, 0x80, 0x80, 0x20, 0x00,
        0x00, 0x80, 0x00, 0xF8, 0x28, 0x24, 0xA0, 0x00,
        0x80, 0x40, 0xA8, 0x20, 0x00, 0xE0, 0x00, 0x40,
        0x90, 0x40, 0x80, 0x10, 0xF0, 0x10, 0x90, 0x20,
        0x90, 0x10, 0x00, 0x00, 0x40, 0xE0, 0x40, 0x40,
        0xA5, 0x00, 0x90, 0x90, 0x80, 0x90, 0x80, 0x80,
        0x90, 0x90, 0x80, 0x20, 0xA0, 0x80, 0x88, 0xA8,
        0x90, 0x80, 0x90, 0x90, 0x10, 0x20, 0x90, 0x50,
        0xA8, 0x90, 0x20, 0x40, 0x80, 0x10, 0x40, 0x00,
        0x00, 0x00, 0x70, 0x90, 0x80, 0x90, 0xF0, 0x40,
        0x90, 0x90, 0x80, 0x40, 0xC0, 0x80, 0x92, 0x90,
        0x90, 0x90, 0x90, 0x80, 0x60, 0x40, 0x90, 0x88,
        0xA8, 0x60, 0x90, 0x40, 0x40, 0x80, 0x40, 0x00,
        0x00, 0x00, 0x00, 0x50, 0x28, 0x2A, 0xA0, 0x00,
        0x80, 0x40, 0x20, 0x20, 0x00, 0x00, 0x00, 0x40,
        0x90, 0x40, 0x80, 0x10, 0x20, 0x10, 0x90, 0x40,
        0x90, 0x10, 0x00, 0x00, 0x20, 0x00, 0x80, 0x00,
        0x9D, 0x00, 0x90, 0x90, 0x80, 0x90, 0x80, 0x80,
        0x90, 0x90, 0x80, 0x20, 0xA0, 0x80, 0x88, 0x98,
        0x90, 0x80, 0x90, 0x90, 0x10, 0x20, 0x90, 0x20,
        0xA8, 0x90, 0x20, 0x80, 0x80, 0x10, 0x40, 0x00,
        0x00, 0x00, 0x90, 0x90, 0x80, 0x90, 0x80, 0x40,
        0x90, 0x90, 0x80, 0x40, 0xA0, 0x80, 0x92, 0x90,
        0x90, 0x90, 0x90, 0x80, 0x10, 0x40, 0x90, 0x50,
        0xA8, 0x90, 0x90, 0x80, 0x40, 0x80, 0x40, 0x00,
        0x00, 0x80, 0x00, 0x50, 0xF0, 0x44, 0x50, 0x00,
        0x80, 0x40, 0x00, 0x00, 0x40, 0x00, 0x80, 0x80,
        0x60, 0x40, 0xF0, 0xE0, 0x20, 0xE0, 0x60, 0x40,
        0x60, 0x60, 0x80, 0x40, 0x00, 0x00, 0x00, 0x40,
        0x42, 0x00, 0x90, 0xE0, 0x70, 0xE0, 0xF0, 0x80,
        0x70, 0x90, 0x80, 0xC0, 0x90, 0xE0, 0x88, 0x88,
        0x60, 0x80, 0x60, 0x90, 0xE0, 0x20, 0x60, 0x20,
        0x50, 0x90, 0x20, 0xF0, 0x80, 0x08, 0x40, 0x00,
        0xE0, 0x00, 0x70, 0xE0, 0x70, 0x70, 0x60, 0x40,
        0x70, 0x90, 0x80, 0x40, 0x90, 0x80, 0x92, 0x90,
        0x60, 0xE0, 0x70, 0x80, 0xE0, 0x60, 0x70, 0x20,
        0x50, 0x90, 0x70, 0xE0, 0x40, 0x80, 0x40, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
        0x40, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0xC0, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x80, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x10, 0x00, 0x20, 0x80, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x60, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x80, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00
    };
}

Painter::Painter()
{

}

void Painter::paint(Widget* widget)
{
    for (auto child : widget->_childWidgets) {
        paint(child);
    }

    printf("Painter::paint: widget=%s\r\n", widget->_name.c_str());

    const auto mappedRect = Rect{
        widget->mapToGlobal(widget->pos()),
        widget->size()
    };

    drawRect(mappedRect);
}

void Painter::setPixel(const int x, const int y, const int color)
{
    if (x < 0 || x >= DisplayWidth || y < 0 || y >= DisplayHeight) {
        return;
    }

    _screenBuf.set(x + y * Display::Width, color > 0);
}

void Painter::drawLine(const int x1, const int y1, const int x2, const int y2, const int color)
{
    printf(
        "Painter::drawLine: x1=%d, y1=%d, x2=%d, y2=%d, color=%d\r\n",
        x1,
        y1,
        x2,
        y2,
        color
    );

    // Horizontal
    if (y1 == y2) {
        for (auto x = x1; x <= x2; ++x) {
            setPixel(x, y1, color);
        }
        return;
    }

    // Vertical
    if (x1 == x2) {
        for (auto y = y1; y <= y2; ++y) {
            setPixel(x1, y, color);
        }

        return;
    }

    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    static const auto plotLineLow = [](Painter* const p, const int x0, const int y0, const int x1, const int y1, const int color) {
        printf(
            "  plotLineLow: x0=%d, y0=%d, x1=%d, y1=%d, color=%d\r\n",
            x0,
            y0,
            x1,
            y1,
            color
        );

        const auto dx = x1 - x0;
        auto dy = y1 - y0;
        auto yi = 1;
        if (dy < 0) {
            yi = -1;
            dy = -dy;
        }
        auto D = 2 * dy - dx;
        auto y = y0;
        for (auto x = x0; x < x1; ++x) {
            p->setPixel(x, y, color);
            if (D > 0) {
                y += yi;
                D += 2 * (dy - dx);
            } else {
                D += 2 * dy;
            }
        }
    };

    static const auto plotLineHigh = [](Painter* const p, const int x0, const int y0, const int x1, const int y1, const int color) {
        printf(
            "  plotLineHigh: x0=%d, y0=%d, x1=%d, y1=%d, color=%d\r\n",
            x0,
            y0,
            x1,
            y1,
            color
        );

        auto dx = x1 - x0;
        const auto dy = y1 - y0;
        auto xi = 1;
        if (dx < 0) {
            xi = -1;
            dx = -dx;
        }
        auto D = 2 * dx - dy;
        auto x = x0;
        for (auto y = y0; y < y1; ++y) {
            p->setPixel(x, y, color);
            if (D > 0) {
                x += xi;
                D += 2 * (dx - dy);
            } else {
                D += 2 * dx;
            }
        }
    };

    if (std::abs(y2 - y1) < std::abs(x2 - x1)) {
        if (x1 > x2) {
            plotLineLow(this, x2, y2, x1, y1, color);
        } else {
            plotLineLow(this, x1, y1, x2, y2, color);
        }
    } else {
        if (y1 > y2) {
            plotLineHigh(this, x2, y2, x1, y1, color);
        } else {
            plotLineHigh(this, x1, y1, x2, y2, color);
        }
    }
}

void Painter::drawEllipse(const int cx, const int cy, const int xRadius, const int yRadius, const int color)
{
    // https://dai.fmph.uniba.sk/upload/0/01/Ellipse.pdf

    static const auto plot4EllipsePoints = [](Painter* const p, const int cx, const int cy, const int x, const int y, const int color) {
        p->setPixel(cx + x, cy + y, color); // Quadrant 1
        p->setPixel(cx - x, cy + y, color); // Quadrant 2
        p->setPixel(cx - x, cy - y, color); // Quadrant 3
        p->setPixel(cx + x, cy - y, color); // Quadrant 4
    };

    const auto twoASquare = 2 * xRadius * xRadius;
    const auto twoBSquare = 2 * yRadius * yRadius;

    // 1st set of points

    auto x = xRadius;
    auto y = 0;

    auto xChange = yRadius * yRadius * (1 - 2 * xRadius);
    auto yChange = xRadius * xRadius;

    auto ellipseError = 0;

    auto stoppingX = twoBSquare * xRadius;
    auto stoppingY = 0;

    while (stoppingX >= stoppingY) {
        plot4EllipsePoints(this, cx, cy, x, y, color);
        ++y;
        stoppingY += twoASquare;
        ellipseError += yChange;
        yChange += twoASquare;
        if (2 * ellipseError + xChange > 0) {
            --x;
            stoppingX -= twoBSquare;
            ellipseError += xChange;
            xChange += twoBSquare;
        }
    }

    // 2nd set of points

    x = 0;
    y = yRadius;

    xChange = yRadius * yRadius;
    yChange = xRadius * xRadius * (1 - 2 * yRadius);
    
    ellipseError = 0;

    stoppingX = 0;
    stoppingY = twoASquare * yRadius;

    while (stoppingX <= stoppingY) {
        plot4EllipsePoints(this, cx, cy, x, y, color);
        ++x;
        stoppingX += twoBSquare;
        ellipseError += xChange;
        xChange += twoBSquare;
        if (2 * ellipseError + yChange > 0) {
            --y;
            stoppingY -= twoASquare;
            ellipseError += yChange;
            yChange += twoASquare;
        }
    }
}

void Painter::drawRect(const Rect& r, const int color)
{
    // TODO implement clipping

    printf(
        "Painter::drawRect: r={ %d;%d %dx%d }, color=%d\r\n",
        r.pos.x,
        r.pos.y,
        r.size.w,
        r.size.h,
        color
    );

    // Top line
    drawLine(
        r.pos.x,
        r.pos.y,
        r.pos.x + r.size.w - 1,
        r.pos.y, 
        color
    );

    // Bottom line
    drawLine(
        r.pos.x,
        r.pos.y + r.size.h - 1,
        r.pos.x + r.size.w - 1,
        r.pos.y + r.size.h - 1,
        color
    );

    // Left line
    drawLine(
        r.pos.x,
        r.pos.y + 1,
        r.pos.x,
        r.pos.y + r.size.h - 2,
        color
    );

    // Right line
    drawLine(
        r.pos.x + r.size.w - 1,
        r.pos.y + 1,
        r.pos.x + r.size.w - 1,
        r.pos.y + r.size.h - 2,
        color   
    );
}

void Painter::fillRect(const Rect& r, const int color)
{
    // TODO implement clipping

    for (auto x = r.pos.x; x < r.pos.x + r.size.w; ++x) {
        if (x < 0 || x >= DisplayWidth) {
            continue;
        }

        for (auto y = r.pos.y; y < r.pos.y + r.size.h; ++y) {
            if (y < 0 || y >= DisplayHeight) {
                continue;
            }
            setPixel(x, y, color);
        }
    }
}

void Painter::drawText(const Rect& r, const std::string& s, int color)
{
    // TODO implement: inversion, clipping, faster data copy

    printf(
        "Painter::drawText: r={ %d;%d %dx%d }, s=%s, color=%d\r\n",
        r.pos.x,
        r.pos.y,
        r.size.w,
        r.size.h,
        s.c_str(),
        color
    );

    const auto* const attributes =
        reinterpret_cast<const FontAttributes*>(Fonts::Default::Attributes);

    const auto* const fontData = Fonts::Default::Data;

    printf(
        "Painter::drawText: bmpBPL=%u, bmpHeight=%u, wideCharCount=%u\r\n",
        attributes->bitmapBytesPerLine,
        attributes->bitmapHeight,
        attributes->wideCharCount
    );

    printf("  wideChars=");
    for (auto i = 0u; i < attributes->wideCharCount; ++i) {
        printf("%u", attributes->wideCharIndices[i]);
        if (i > 0) {
            printf(",");
        }
    }
    printf("\r\n");
    
    // const auto invert = false;
    const auto rightJustified = false;

    Rect calculatedRect = r;

    // Adjust X coordinate for right justified text
    if (rightJustified) {
        for (const auto c : s) {
            const auto charIdx = c - 32;
            if (charIdx >= FontAttributes::CharacterCount) {
                continue;
            }
            calculatedRect.pos.x -= attributes->characterWidths[charIdx];
        }
    }

    printf(
        "Painter::drawText: calculatedRect={ %d;%d %dx%d }\r\n",
        calculatedRect.pos.x,
        calculatedRect.pos.y,
        calculatedRect.size.w,
        calculatedRect.size.h
    );

    auto x = calculatedRect.pos.x;

    for (const auto c : s) {
        printf("  c=%c", c);

        const unsigned charIdx = c - 32;
        printf(",ci=%d", charIdx);
        if (charIdx >= FontAttributes::CharacterCount) {
            continue;
        }

        const auto charWidth = attributes->characterWidths[charIdx];
        printf(",cw=%u", charWidth);
        if (charWidth == 0) {
            continue;
        }
        const auto charHeight = attributes->bitmapHeight;
        const auto charBytes = charWidth > 8 ? 2u : 1u;
        printf(",ch=%u,cb=%u", charHeight, charBytes);

        // Unused at the moment
        // const auto maskBits = charBytes == 1 ? 8 - charWidth : 16 - charWidth;

        auto charDataIdx = 0u;
        // Handle zero-width characters
        for (auto i = 0u; i < charIdx; ++i) {
            if (attributes->characterWidths[i] > 0) {
                ++charDataIdx;
            }
        }
        // Count how many wide characters we passed
        for (auto i = 0u; i < attributes->wideCharCount; ++i) {
            if (charIdx > attributes->wideCharIndices[i]) {
                printf(",++cdi");
                ++charDataIdx;
            }
        }
        printf(",cdi=%u", charDataIdx);

        printf("\r\n");

        // Copy character data
        // FIXME this is a naive implementation which copies pixel-by-pixel
        auto y = calculatedRect.pos.y;
        for (auto line = 0u; line < charHeight; ++line) {
            for (auto charByte = 0u; charByte < charBytes; ++charByte) {
                const auto charDataAddr = charDataIdx + charByte + line * attributes->bitmapBytesPerLine;
                printf("    cda=%u", charDataAddr);
                const auto charData = fontData[charDataAddr];
                printf(",cd=%02Xh", charData);
                for (auto k = 0u; k < 8; ++k) {
                    // Don't draw pixels outside of our rect
                    if (!r.contains(Point{ x + k, y })) {
                        continue;
                    }

                    // Test if the pixel is set
                    if ((charData & (1 << (7 - k))) > 0) {
                        printf(",{k=%u,x=%d,y=%d}", k, x + k, y);
                        setPixel(x + k, y, color);
                    }
                }
                printf("\r\n");
            }
            ++y;
        }
        x += charWidth;
    }
}

void Painter::updateDisplay()
{
    const auto lineHeight = Display::Height / Display::Lines;
    for (auto row = 0; row < DisplayLines; ++row) {
        Display::setColumn(0);
        Display::setLine(row);
        std::array<uint8_t, Display::Width> colData{};
        for (auto col = 0; col < DisplayWidth; ++col) {
            for (auto i = 0u; i < 8; ++i) {
                if (_screenBuf.test(col + (row * lineHeight + i) * Display::Width)) {
                    colData[col] |= 1 << i;
                }
            }
        }
        Display::sendData(colData.data(), colData.size());
    }
}

void Painter::clearBuffer()
{
    _screenBuf = {};
}