/*
 * DiagTerminal.h
 *
 *  Created on: 2018. máj. 27.
 *      Author: tkarpati
 */

#ifndef HUB_DIAGTERMINAL_H_
#define HUB_DIAGTERMINAL_H_

#ifdef __cplusplus

#include <stdint.h>

class DiagTerminal
{
public:
    static DiagTerminal* instance();

    enum class Action
    {
        Nothing,

        ReadStatus,

        Shutter1Up,
        Shutter1Down,
        Shutter2Up,
        Shutter2Down,

        ScanUnits,
        SelectDevice
    };

    Action task();
    void onUSARTDataReceived(char c);

    uint8_t selectedDeviceIndex();

private:
    DiagTerminal();

    enum class State
    {
        InMainMenu,
        InSendCommandMenu,
        InSelectDeviceMenu
    };

    Action mAction = Action::Nothing;
    State mState = State::InMainMenu;
    bool mMenuUpdateNeeded = true;
    bool mUSARTInterruptRequest = false;
    const char* mValidOptions = nullptr;
    uint8_t mSelectedDeviceIndex = 0;

    void printMenu();
    void printPrompt(const char* validOptions = nullptr);
    void printMainMenu();
    void printSendCommandMenu();
    void printSelectDeviceMenu();

    void selectOption(char option);
};

extern "C" {
#endif

// Called directly from ISR
void DiagTerminal_usartDataReceived(char c);

#ifdef __cplusplus
}
#endif

#endif /* HUB_DIAGTERMINAL_H_ */
