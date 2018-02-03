#include "ui.hpp"

#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0);

namespace ui
{

UiHandler::UiHandler()
{
    oled.clearDisplay();
    oled.clearBuffer();
}

void UiHandler::task(drivers::Keypad::Event keypadEvent)
{
    
}

}