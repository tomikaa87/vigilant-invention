#pragma once

#include <cstdint>

class BlynkParam;

class BlynkHandler
{
public:
    BlynkHandler();
    ~BlynkHandler();

    void task();

    void updateTemperature(const int16_t value);

    void onConnected();
    void onButtonPressed(const int pin);
    void onVirtualPinUpdated(const int pin, const BlynkParam& param);
    void updateVirtualPin(const int pin);

private:
    int16_t m_lastRoomTemperature = 0;
};