#pragma once

#include "IdeHelper.h"

class SwitchModule final
{
public:
    SwitchModule();

    void setState(bool on);
    bool state() const;

private:
    bool m_state = false;
};