#pragma once

#include "drivers/keypad.hpp"

namespace ui
{

class UiHandler
{
public:
    UiHandler();

    void task(drivers::Keypad::Event keypadEvent);
};

}