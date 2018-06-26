/*
 * DiagTerminal.cpp
 *
 *  Created on: 2018. máj. 27.
 *      Author: tkarpati
 */

#include <hub/DiagTerminal.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <stm32f4xx.h>

DiagTerminal::DiagTerminal()
{
}

DiagTerminal* DiagTerminal::instance()
{
    static DiagTerminal* sInstance = new DiagTerminal();
    return sInstance;
}

DiagTerminal::Action DiagTerminal::task()
{
    if (mMenuUpdateNeeded)
    {
        mMenuUpdateNeeded = false;
        printMenu();
    }

    if (mAction != Action::Nothing)
    {
        auto action = mAction;
        mAction = Action::Nothing;
        return action;
    }

    return Action::Nothing;
}

void DiagTerminal::printPrompt(const char* validOptions)
{
    printf("Selection");

    if (validOptions)
    {
        printf(" (%s)", validOptions);
        mValidOptions = validOptions;
    }

    printf(": ");
}

void DiagTerminal::printMenu()
{
    printf("\r\n");

    switch (mState)
    {
        case State::InMainMenu:
            printMainMenu();
            break;

        case State::InSendCommandMenu:
            printSendCommandMenu();
            break;

        case State::InSelectDeviceMenu:
            printSelectDeviceMenu();
            break;

        default:
            break;
    }
}

void DiagTerminal::onUSARTDataReceived(char c)
{
    if (isalnum(c))
    {
        // Process incoming character as a choice
        printf("%c", c);

        if (mValidOptions && strchr(mValidOptions, c))
        {
            printf("\r\n");
            selectOption(c);
        }
        else
        {
            printf("\r\nInvalid option selected\r\n");
            printMenu();
        }
    }
    else if (c == '\r')
    {
        printMenu();
    }
}

void DiagTerminal::printMainMenu()
{
    printf("++ SMR Hub Diagnostic Menu ++\r\n");
    printf("1) Scan SMRR units\r\n");
    printf("2) Select SMRR unit\r\n");
    printf("3) Read status\r\n");
    printf("4) Send command\r\n");

    printPrompt("1234");
}

void DiagTerminal::printSendCommandMenu()
{
    printf("++ Send Command Menu ++\r\n");
    printf("1) Shutter 1 Up\r\n");
    printf("2) Shutter 1 Down\r\n");
    printf("3) Shutter 2 Up\r\n");
    printf("4) Shutter 2 Down\r\n");
    printf("5) Back\r\n");

    printPrompt("12345");
}

void DiagTerminal_usartDataReceived(char c)
{
    DiagTerminal::instance()->onUSARTDataReceived(c);
}

void DiagTerminal::printSelectDeviceMenu()
{
    printf("++ Select Device Menu ++\r\n");
    printf("0) SMRR0\r\n");
    printf("1) SMRR1\r\n");
    printf("2) SMRR2\r\n");
    printf("3) SMRR3\r\n");
    printf("4) SMRR4\r\n");
    printf("5) SMRR5\r\n");
    printf("6) SMRR6\r\n");
    printf("7) SMRR7\r\n");
    printf("8) SMRR8\r\n");
    printf("9) SMRR9\r\n");
    printf("b) Back\r\n");

    printPrompt("0123456789b");
}

uint8_t DiagTerminal::selectedDeviceIndex()
{
    return mSelectedDeviceIndex;
}

void DiagTerminal::selectOption(char option)
{
    switch (mState)
    {
        case State::InMainMenu:
        {
            switch (option)
            {
                case '1':
                    mAction = Action::ScanUnits;
                    break;

                case '2':
                    mState = State::InSelectDeviceMenu;
                    printMenu();
                    break;

                case '3':
                    printf("Reading status\r\n");
                    mAction = Action::ReadStatus;
                    break;

                case '4':
                    mState = State::InSendCommandMenu;
                    printMenu();
                    break;

                default:
                    break;
            }

            break;
        }

        case State::InSendCommandMenu:
        {
            switch (option)
            {
                case '1':
                    mAction = Action::Shutter1Up;
                    break;

                case '2':
                    mAction = Action::Shutter1Down;
                    break;

                case '3':
                    mAction = Action::Shutter2Up;
                    break;

                case '4':
                    mAction = Action::Shutter2Down;
                    break;

                case '5':
                    mState = State::InMainMenu;
                    printMenu();
                    break;

            }
        }

        case State::InSelectDeviceMenu:
        {
            if (option >= '0' && option <= '9')
            {
                mAction = Action::SelectDevice;
                mSelectedDeviceIndex = option - '0';
                mState = State::InMainMenu;
                mMenuUpdateNeeded = true;
            }
            else if (option == 'b')
            {
                mState = State::InMainMenu;
                mMenuUpdateNeeded = true;
            }

            break;
        }

        default:
            break;
    }
}
