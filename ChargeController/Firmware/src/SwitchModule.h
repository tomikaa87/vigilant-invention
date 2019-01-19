#pragma once

class SwitchModule
{
public:
    SwitchModule();

    void setState(bool on);
    bool state() const;

private:
    bool m_state = false;
};