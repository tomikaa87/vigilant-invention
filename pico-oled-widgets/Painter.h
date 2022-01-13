#pragma once

#include "Display.h"

#include <bitset>
#include <string>

struct Rect;
class Widget;

class Painter
{
public:
    explicit Painter();

    void paint(Widget* widget);

    void setPixel(int x, int y, int color);

    void drawLine(int x1, int y1, int x2, int y2, int color = 1);
    void drawEllipse(int cx, int cy, int xRadius, int yRadius, int color = 1);

    void drawRect(const Rect& r, int color = 1);
    void fillRect(const Rect& r, int color = 1);

    void drawText(const Rect& r, const std::string& s, int color = 1);

    void updateDisplay();
    void clearBuffer();

private:
    static constexpr auto DisplayWidth = static_cast<int>(Display::Width);
    static_assert(DisplayWidth > 0);
    static constexpr auto DisplayHeight = static_cast<int>(Display::Height);
    static_assert(DisplayHeight > 0);
    static constexpr auto DisplayLines = static_cast<int>(Display::Lines);
    static_assert(DisplayLines > 0);

    struct FontAttributes {
        static constexpr auto CharacterCount = 95;
        uint8_t characterWidths[CharacterCount];
        uint8_t bitmapBytesPerLine;
        uint8_t bitmapHeight;
        uint8_t wideCharCount;
        uint8_t* wideCharIndices;
    } __attribute__((packed));
    static_assert(
        sizeof(FontAttributes) == 
            FontAttributes::CharacterCount
            + sizeof(uint8_t) * 3
            + sizeof(uint8_t *)
    );

    std::bitset<Display::Width * Display::Height> _screenBuf;
};