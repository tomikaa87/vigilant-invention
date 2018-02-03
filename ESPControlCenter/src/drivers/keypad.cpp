#include "keypad.hpp"

namespace drivers
{

Keypad::Keypad()
{
    static_assert(sizeof(m_buttonStates) == 1, "size of m_buttonStates should be 1 byte");

    button1::setup();
    button2::setup();
    button3::setup();
    button4::setup();
}

Keypad::Event Keypad::task()
{
    return Event::Nothing;
}

}