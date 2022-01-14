/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC16LF15313
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define CLOCK_TICKS_PER_SECOND      (100)
#define LED_TIMER_TEST_MODE         1

#if LED_TIMER_TEST_MODE == 1
#define LED_TIMER_HOUR_MULTIPLIER   (1)
#else
#define LED_TIMER_HOUR_MULTIPLIER   (3600)
#endif

volatile uint8_t clockTicks = 0;
volatile uint32_t clockSeconds = 0;

//
// Settings implementation
//

#define SETTINGS_START_ADDRESS      (END_FLASH - 128)
#define SETTINGS_FLASH_BLOCK_COUNT  (1 + sizeof(struct Settings) / WRITE_FLASH_BLOCKSIZE)
#define SETTINGS_HEADER_MAGIC       (0xAB)

const uint8_t BrightnessPWMValues[5] = {
    6, 12, 18, 24, 32
};

struct Settings {
    uint8_t checksum;

    uint8_t headerMagic;

    uint8_t osctuneValue;
    uint8_t onTimeHours;
    uint8_t brightness;
} settings;

const struct Settings DefaultSettings = {
    .checksum = 0,

    .headerMagic = SETTINGS_HEADER_MAGIC,

    .osctuneValue = 10,
    .onTimeHours = 6,
    .brightness = 2
};

uint8_t Settings__calculateChecksum(const struct Settings* const s)
{
    const uint8_t* p = (const uint8_t*) s + sizeof(s->checksum);
    uint8_t len = sizeof(struct Settings) - sizeof(s->checksum);

    // Standard CRC-8
    const uint8_t Init = 0x00;
    const uint8_t Poly = 0x07;
    int crc = Init;
    while (len--) {
        crc ^= *p++;
        for (uint8_t i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ Poly : crc << 1;
    }

    return crc & 0xff;
}

void Settings_Save()
{
    uint16_t buf[WRITE_FLASH_BLOCKSIZE * SETTINGS_FLASH_BLOCK_COUNT];

    settings.headerMagic = SETTINGS_HEADER_MAGIC;
    settings.checksum = Settings__calculateChecksum(&settings);

    memset(buf, 0, sizeof(buf));

    for (uint8_t i = 0; i < sizeof(struct Settings); ++i) {
        buf[i] = ((const uint8_t*)&settings)[i];
    }

    FLASH_WriteBlock(SETTINGS_START_ADDRESS, buf);
}

void Settings_Load()
{
    memset(&settings, 0, sizeof(struct Settings));

    for (uint8_t i = 0; i < sizeof(struct Settings); ++i) {
        ((uint8_t*)&settings)[i] = FLASH_ReadWord(SETTINGS_START_ADDRESS + i) & 0xFF;
    }

    if (settings.checksum != Settings__calculateChecksum(&settings)
        || settings.headerMagic != SETTINGS_HEADER_MAGIC
    ) {
        settings = DefaultSettings;
        settings.checksum = Settings__calculateChecksum(&settings);
        Settings_Save();
    }
}

//
// End of Settings implementation
//

//
// Menu implementation
// Navigation:
//      Short press: step current value
//      Long press: navigate to the next item and exit at the end
//

#define MENU_VALUE_COUNT        (3)

typedef enum {
    MENU_CHANGE_REASON_VALUE_CHANGED,
    MENU_CHANGE_REASON_INDEX_CHANGED
} MenuChangeReason;

void Menu_OnChanged(MenuChangeReason reason);
void Menu_OnExited();

struct Menu {
    uint8_t currentIndex;
    struct MenuValue {
        uint8_t current;
        uint8_t minimum;
        uint8_t maximum;
    } values[MENU_VALUE_COUNT];
} menu;

typedef enum {
    MENU_BUTTON_PRESS_SHORT,
    MENU_BUTTON_PRESS_LONG
} MenuButtonEvent;

void Menu_Initialize()
{
    memset(&menu, 0, sizeof(struct Menu));
}

void Menu_SetValueWithRange(const uint8_t index, const uint8_t value, const uint8_t minimum, const uint8_t maximum)
{
    if (index >= MENU_VALUE_COUNT) {
        return;
    }

    struct MenuValue* v = &menu.values[index];

    v->current = value;
    v->minimum = minimum;
    v->maximum = maximum;

    if (v->current < minimum) {
        v->current = minimum;
    } else if (v->current > maximum) {
        v->current = maximum;
    }
}

uint8_t Menu_GetCurrentValue()
{
    return menu.values[menu.currentIndex].current;
}

uint8_t Menu_GetCurrentIndex()
{
    if (menu.currentIndex >= MENU_VALUE_COUNT) {
        return 0;
    }

    return menu.currentIndex;
}

void Menu_HandleButtonEvent(const MenuButtonEvent event)
{
    switch (event) {
    case MENU_BUTTON_PRESS_SHORT: {
        struct MenuValue* v = &menu.values[menu.currentIndex];

        ++v->current;
        if (v->current > v->maximum) {
            v->current = v->minimum;
        }

        Menu_OnChanged(MENU_CHANGE_REASON_VALUE_CHANGED);
        break;
    }

    case MENU_BUTTON_PRESS_LONG: {
        ++menu.currentIndex;
        if (menu.currentIndex >= MENU_VALUE_COUNT) {
            menu.currentIndex = 0;
            Menu_OnExited();
        } else {
            Menu_OnChanged(MENU_CHANGE_REASON_INDEX_CHANGED);
        }
        break;
    }

    default:
        break;
    }
}

void Menu_Enter()
{
    menu.currentIndex = 0;
    Menu_OnChanged(MENU_CHANGE_REASON_INDEX_CHANGED);
}

//
// End of Menu implementation
//

//
// LED output control implementation
//

void LED_OnBlinkingFinished(uint8_t arg);

struct LED {
    enum {
        LED_OFF,
        LED_STEADY,
        LED_BLINKING,
        LED_BLINKING_SINGLE_SHOT
    } state;
    uint8_t blinkCount;
    uint8_t startBlinkCount;
    uint8_t blinkFinishedArg;
    uint8_t blinkRemainingTime;
    bool blinkLong;
    uint32_t stateSwitchSeconds;
    bool controlledByTimer;
    bool stateByTimer;
} led;

void LED__loadTimerForState(const bool state)
{
    if (state) {
        // 'ON' timer, load (ON time) seconds
        led.stateSwitchSeconds = clockSeconds + settings.onTimeHours * LED_TIMER_HOUR_MULTIPLIER;
    } else {
        // 'OFF' timer, load (24h - ON time) seconds
        led.stateSwitchSeconds = clockSeconds + 24 * LED_TIMER_HOUR_MULTIPLIER - settings.onTimeHours * LED_TIMER_HOUR_MULTIPLIER;
    }
}

void LED_Initialize()
{
    memset(&led, 0, sizeof(struct LED));
}

void LED_SwitchOff()
{
    led.state = LED_OFF;
    PWM3_LoadDutyValue(0);
}

void LED_SwitchOn()
{
    led.state = LED_STEADY;
    PWM3_LoadDutyValue(BrightnessPWMValues[settings.brightness]);
}

void LED_Blink(const uint8_t count)
{
    if (count > 127) {
        return;
    }

    led.state = LED_BLINKING;
    led.blinkCount = count * 2;
    led.startBlinkCount = led.blinkCount;

    // Start with a short pause
    PWM3_LoadDutyValue(0);
    led.blinkRemainingTime = 25;

    led.controlledByTimer = false;
}

void LED_BlinkSingleShot(const uint8_t count, const bool blinkLong)
{
    LED_Blink(count);
    led.state = LED_BLINKING_SINGLE_SHOT;
    led.blinkLong = blinkLong;
}

bool LED_IsOn()
{
    return led.state != LED_OFF;
}

void LED_SetControlledByTimer(const bool enabled)
{
    led.controlledByTimer = enabled;
}

void LED_SetStateByTimer(const bool on)
{
    led.stateByTimer = on;
    LED__loadTimerForState(on);
}

bool LED_GetStateByTimer()
{
    return led.stateByTimer;
}

void LED_HandleTimerTick()
{
    if (led.blinkRemainingTime > 0) {
        --led.blinkRemainingTime;
    }
}

void LED_Task()
{
    switch (led.state) {
    case LED_BLINKING_SINGLE_SHOT:
    case LED_BLINKING: {
        if (led.blinkRemainingTime > 0) {
            break;
        }

        if (led.blinkCount > 0) {
            if (led.blinkCount % 2 == 0) {
                // Start a blink on even cound
                led.blinkRemainingTime = led.blinkLong ? 100 : 25;
                PWM3_LoadDutyValue(BrightnessPWMValues[settings.brightness]);
            } else {
                // Pause on odd count
                if (led.blinkCount > 1) {
                    led.blinkRemainingTime = 25;
                } else {
                    // Long pause at the end
                    led.blinkRemainingTime = 200;
                }
                PWM3_LoadDutyValue(0);
            }
            --led.blinkCount;
        } else {
            if (led.state == LED_BLINKING) {
                // Restart blinking
                led.blinkCount = led.startBlinkCount;
            } else {
                LED_SwitchOff();
                LED_OnBlinkingFinished(led.blinkFinishedArg);
            }
        }
        break;
    }

    case LED_STEADY:
    default:
        break;
    }

    if (clockSeconds >= led.stateSwitchSeconds) {
        led.stateByTimer = !led.stateByTimer;
        LED__loadTimerForState(led.stateByTimer);
    }

    if (led.controlledByTimer) {
        if (led.stateByTimer && led.state != LED_STEADY) {
            LED_SwitchOn();
        } else if (!led.stateByTimer && led.state != LED_OFF) {
            LED_SwitchOff();
        }
    }
}

void LED_SetBlinkFinishedArg(const uint8_t arg)
{
    led.blinkFinishedArg = arg;
}

//
// End of LED output control implementation
//

//
// Button handler implementation
//

void Button_OnPressed(bool longPress);

struct Button {
    enum {
        BUTTON_RELEASED,
        BUTTON_PRESSED,
        BUTTON_LONG_PRESSED
    } state;
    volatile uint8_t stateChangeElapsedTime;
    volatile bool inputActive;
    uint8_t inputFilter;
} button;

void Button_Initialize()
{
    memset(&button, 0, sizeof(struct Button));
}

void Button_HandleInterrupt()
{
    button.inputActive = IO_BTN_GetValue() == 0;
}

void Button_HandleTimerTick()
{
    if (button.stateChangeElapsedTime < 255) {
        ++button.stateChangeElapsedTime;
    }
}

void Button_Task()
{
    // Simple filtering for bouncing
    if (button.inputActive && button.inputFilter < 255) {
        ++button.inputFilter;
    } else if (!button.inputActive && button.inputFilter > 0) {
        --button.inputFilter;
    }

    // Adjust the thresholds if the button is bouncing or responding slowly
    bool inputActive = button.inputFilter > 200;
    bool inputInactive = button.inputFilter < 50;
    bool inputStable = inputActive ^ inputInactive;

    if (inputStable) {
        if (inputActive && button.state == BUTTON_RELEASED) {
            button.stateChangeElapsedTime = 0;
            button.state = BUTTON_PRESSED;
        } else if (inputActive && button.state == BUTTON_PRESSED && button.stateChangeElapsedTime > 100) {
            button.state = BUTTON_LONG_PRESSED;
            // Trigger the event immediately instead of waiting for release
            Button_OnPressed(true);
        } else if (inputInactive && button.state != BUTTON_RELEASED) {
            if (button.state != BUTTON_LONG_PRESSED) {
                Button_OnPressed(false);
            }
            button.state = BUTTON_RELEASED;
        }
    }
}

//
// End of Button handler implementation
//

struct MainState {
    bool inMenu;
} state;

enum {
    MENU_INDEX_ON_TIME,
    MENU_INDEX_BRIGHTNESS,
    MENU_INDEX_OSCTUNE
};

void setOsctune(uint8_t value)
{
    int8_t offset = (int8_t)value - 10;
    OSCTUNE = offset & 0b111111;
}

void menuValueChanged()
{
    switch (Menu_GetCurrentIndex()) {
    case MENU_INDEX_ON_TIME: {
        settings.onTimeHours = Menu_GetCurrentValue();
        LED_Blink(settings.onTimeHours);
        break;
    }

    case MENU_INDEX_BRIGHTNESS: {
        settings.brightness = Menu_GetCurrentValue();
        LED_SwitchOn();
        break;
    }

    case MENU_INDEX_OSCTUNE: {
        settings.osctuneValue = Menu_GetCurrentValue();
        setOsctune(settings.osctuneValue);
        LED_Blink(settings.osctuneValue);
        break;
    }

    default:
        break;
    }
}

void LED_OnBlinkingFinished(const uint8_t arg)
{
    switch (arg) {
    case MENU_INDEX_ON_TIME:
    case MENU_INDEX_BRIGHTNESS:
    case MENU_INDEX_OSCTUNE:
        menuValueChanged();
        break;

    default:
        break;
    }
}

void Menu_OnChanged(const MenuChangeReason reason)
{
    switch (reason) {
    case MENU_CHANGE_REASON_VALUE_CHANGED:
        menuValueChanged();
        break;

    case MENU_CHANGE_REASON_INDEX_CHANGED:
        LED_SetBlinkFinishedArg(MENU_CHANGE_REASON_INDEX_CHANGED);
        LED_BlinkSingleShot(Menu_GetCurrentIndex() + 1, false);
        break;

    default:
        break;
    }
}

void Menu_OnExited()
{
    state.inMenu = false;

    LED_SetBlinkFinishedArg(255);
    LED_BlinkSingleShot(2, true);

    Settings_Save();

    LED_SetControlledByTimer(true);
}

void Button_OnPressed(const bool longPress)
{
    if (state.inMenu) {
            Menu_HandleButtonEvent(longPress ? MENU_BUTTON_PRESS_LONG : MENU_BUTTON_PRESS_SHORT);
    } else {
        if (longPress) {
            state.inMenu = true;
            LED_SetControlledByTimer(true);
            Menu_Enter();
        } else {
            LED_SetStateByTimer(!LED_GetStateByTimer());
        }
    }
}

void timer0Isr()
{
    IO_CLK_Toggle();
    Button_HandleTimerTick();
    LED_HandleTimerTick();

    if (++clockTicks >= CLOCK_TICKS_PER_SECOND) {
        clockTicks = 0;
        ++clockSeconds;
    }
}

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    TMR0_SetInterruptHandler(timer0Isr);

    memset(&state, 0, sizeof(struct MainState));

    Settings_Load();

    setOsctune(settings.osctuneValue);

    Menu_Initialize();

    // "On Time (hours)" control
    Menu_SetValueWithRange(MENU_INDEX_ON_TIME, settings.onTimeHours, 0, 24);

    // "Brightness" control
    Menu_SetValueWithRange(MENU_INDEX_BRIGHTNESS, settings.brightness, 0, 4);

    // "OSCTUNE values" control
    Menu_SetValueWithRange(MENU_INDEX_OSCTUNE, settings.osctuneValue, 0, 20);

    Button_Initialize();
    IOCAF5_SetInterruptHandler(Button_HandleInterrupt);

    // Turn on the LED by default
    LED_Initialize();
    LED_SetControlledByTimer(true);
    LED_SetStateByTimer(true);

    while (1)
    {
        Button_Task();
        LED_Task();
    }
}
/**
 End of File
*/