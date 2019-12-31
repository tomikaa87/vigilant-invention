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
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC16F1825
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

/*
 * Enyhen nedves: 480
 * Frissen ontozott: 419
 */

#include "mcc_generated_files/mcc.h"

#include "ssd1306.h"
#include "text.h"

#include <stdbool.h>
#include <stdio.h>

#include <xc.h>
#include <string.h>

typedef enum {
        SENSOR_IDLE,
        SENSOR_PWR_UP,
        SENSOR_READY
} sensor_state_t;

struct {
        uint8_t state: 2;
        uint8_t warn;
        uint8_t: 0;
        uint16_t timer;
        uint16_t adc_min;
        uint16_t adc_max;
        uint16_t last_adc;
        uint32_t meas_cnt;
        uint32_t meas_cnt_slp;
} sensor;

struct {
        uint16_t magic;
        uint16_t adc_warn;
        uint16_t adc_warn_off;
} settings;

#define BTN_DEBOUNCE_DELAY      (10)
#define BTN_LONG_PRESS_DELAY    (500)

struct {
        uint8_t pressed: 1;
        uint8_t isr: 1;
        uint8_t hold: 1;
        uint8_t can_repeat: 1;
        uint8_t: 0;

        uint16_t timer;
        uint16_t hold_delay;
} button;

typedef enum {
        SCR_MAIN,
        SCR_MENU
} scr_t;

struct {
        uint8_t pwr_on: 1;
        uint8_t scr: 2;
        uint8_t menu_item: 3;
        uint8_t: 0;

        uint16_t timer;
} display;

struct {
        uint8_t sleep: 1;
        uint8_t: 0;
} power;

struct {
        uint8_t on: 1;
        uint8_t: 0;

        uint16_t len;
        uint16_t timer;
} beeper;

void tmr_isr()
{
        if (sensor.timer < 0xffff)
                ++sensor.timer;

        if (button.timer < 0xffff)
                ++button.timer;

        if (display.timer < 0xffff)
                ++display.timer;

        if (beeper.timer < 0xffff)
                ++beeper.timer;
}

void btn_isr()
{
        button.isr = 1;
}

bool is_btn_pressed()
{
        return IO_BUTTON1_GetValue() == 0;
}

typedef enum {
        BTN_IDLE,
        BTN_PRESSED,
        BTN_PRESSED_LONG
} btn_state_t;

btn_state_t read_btn()
{
        if (button.isr || is_btn_pressed()) {
                button.isr = 0;

                // If woken up from sleep, immediately trigger a press event
                if (power.sleep) {
                        return BTN_PRESSED;
                }

                // Button pressed
                if (!button.pressed && button.timer >= BTN_DEBOUNCE_DELAY) {
                        button.pressed = 1;
                        button.timer = 0;
                        button.hold_delay = BTN_LONG_PRESS_DELAY;
                }

                // Button held in
                if ((!button.hold || button.can_repeat) && button.pressed && button.timer >= button.hold_delay) {
                        if (button.hold && button.hold_delay > 25)
                                button.hold_delay -= 25;

                        button.hold = 1;
                        button.timer = 0;
                        return BTN_PRESSED_LONG;
                }
        } else {
                // Button released
                if (button.pressed && button.timer >= BTN_DEBOUNCE_DELAY) {
                        button.pressed = 0;
                        button.timer = 0;

                        if (!button.hold)
                                return BTN_PRESSED;

                        button.hold = 0;
                }
        }

        return BTN_IDLE;
}

void beep(uint16_t length)
{
        if (!beeper.on) {
                beeper.on = 1;
                beeper.len = length;
                beeper.timer = 0;
                CCP1CON |= 0b00001100;
        }
}

void beeper_task()
{
        if (beeper.on && beeper.timer >= beeper.len) {
                beeper.on = 0;
                CCP1CON &= 0b11110000;
        }
}

void settings_load()
{
        uint8_t* p = &settings;
        for (uint8_t i = 0; i < sizeof(settings); ++i)
                p[i] = DATAEE_ReadByte(i);

        if (settings.magic != 0x1234) {
                settings.adc_warn = 1023;
                settings.adc_warn_off = 1023;
        }
}

void settings_save()
{
        settings.magic = 0x1234;

        const uint8_t* p = &settings;
        for (uint8_t i = 0; i < sizeof(settings); ++i)
                if (DATAEE_ReadByte(i) != p[i])
                        DATAEE_WriteByte(i, p[i]);
}

void show_scr_main()
{
        display.scr = SCR_MAIN;

        ssd1306_clear();

        text_draw("FLOWER GUARD V1.0", 0, 0, 0);
        text_draw("ADC VALUE:", 2, 0, 0);
        text_draw("ADC MIN  :", 3, 0, 0);
        text_draw("ADC MAX  :", 4, 0, 0);
        text_draw("MCNT     :", 5, 0, 0);
        text_draw("MCNT(SLP):", 6, 0, 0);
}

void upd_scr_main()
{
        char s[10] = { 0 };

        sprintf(s, "%5u", sensor.last_adc);
        text_draw(s, 2, 65, 0);
        sprintf(s, "%5u", sensor.adc_min);
        text_draw(s, 3, 65, 0);
        sprintf(s, "%5u", sensor.adc_max);
        text_draw(s, 4, 65, 0);
        sprintf(s, "%lu", sensor.meas_cnt);
        text_draw(s, 5, 65, 0);
        sprintf(s, "%lu", sensor.meas_cnt_slp);
        text_draw(s, 6, 65, 0);

        if (sensor.warn) {
                text_draw("WATER NEEDED!", 7, 0, 0);
        } else {
                text_draw("             ", 7, 0, 0);
        }
}

void show_scr_menu()
{
        display.scr = SCR_MENU;

        ssd1306_clear();

        uint8_t line = 0;

        text_draw("WARN. ADC   :", line++, TEXT_NORMAL_CHAR_WIDTH * 2, 0);
        text_draw("WARN OFF ADC:", line++, TEXT_NORMAL_CHAR_WIDTH * 2, 0);
        text_draw("RESET MIN/MAX", line++, TEXT_NORMAL_CHAR_WIDTH * 2, 0);
        text_draw("APPLY", line++, 10, 0);
        text_draw("CANCEL", line++, 10, 0);
}

void upd_scr_menu()
{
        button.can_repeat = display.menu_item < 2;

        for (uint8_t i = 0; i < 5; ++i) {
                text_draw(display.menu_item == i ? ">" : " ", i, 0, 0);
        }

        char s[12] = { 0 };

        sprintf(s, "%u     ", settings.adc_warn);
        text_draw(s, 0, TEXT_NORMAL_CHAR_WIDTH * 18, 0);

        sprintf(s, "%u     ", settings.adc_warn_off);
        text_draw(s, 1, TEXT_NORMAL_CHAR_WIDTH * 18, 0);
}

void menu_next_item()
{
        if (++display.menu_item > 4)
                display.menu_item = 0;

        upd_scr_menu();
}

void menu_step_sel()
{
        switch (display.menu_item) {
        case 0:
                if (++settings.adc_warn > 1023)
                        settings.adc_warn = 0;
                upd_scr_menu();
                break;

        case 1:
                if (++settings.adc_warn_off > 1023)
                        settings.adc_warn_off = 0;
                upd_scr_menu();
                break;
        case 2:
                sensor.adc_max = 0;
                sensor.adc_min = 0xffff;
                show_scr_main();
                break;
        case 3:
                settings_save();
                // no break
        case 4:
                show_scr_main();
                break;
        }
}

/*
                         Main application
 */
void main(void)
{
        // initialize the device
        SYSTEM_Initialize();

        CCP1CON &= 0b11110000;

        // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
        // Use the following macros to:

        // Enable the Global Interrupts
        INTERRUPT_GlobalInterruptEnable();

        // Enable the Peripheral Interrupts
        INTERRUPT_PeripheralInterruptEnable();

        // Disable the Global Interrupts
        //INTERRUPT_GlobalInterruptDisable();

        // Disable the Peripheral Interrupts
        //INTERRUPT_PeripheralInterruptDisable();

        memset(&sensor, 0, sizeof(sensor));
        memset(&button, 0, sizeof(button));
        memset(&display, 0, sizeof(display));
        memset(&power, 0, sizeof(power));
        memset(&beeper, 0, sizeof(beeper));

        sensor.adc_min = 0xffff;

        settings_load();

        TMR1_SetInterruptHandler(tmr_isr);
        IOCAF5_SetInterruptHandler(btn_isr);

        IO_SENSOR_PWR_SetLow();

        ssd1306_init();
        ssd1306_set_brightness(10);
        display.pwr_on = 1;

        show_scr_main();

        while (1) {
                CLRWDT();

                if (power.sleep && sensor.state == SENSOR_IDLE && !beeper.on) {
                        sensor.timer = 500;
                }

                beeper_task();

                btn_state_t btn = read_btn();

                switch (btn)
                {
                case BTN_IDLE:
                        break;

                case BTN_PRESSED:
                        beep(50);

                        display.timer = 0;
                        power.sleep = 0;

                        if (!display.pwr_on) {
                                display.pwr_on = 1;
                                ssd1306_send_command(SSD1306_CMD_DISPLAYON);
                        } else {
                                switch (display.scr) {
                                case SCR_MENU:
                                        menu_next_item();
                                        break;
                                default:
                                        break;
                                }
                        }
                        break;

                case BTN_PRESSED_LONG:
                        display.timer = 0;
                        power.sleep = 0;

                        switch (display.scr) {
                        case SCR_MAIN:
                                show_scr_menu();
                                break;
                        case SCR_MENU:
                                menu_step_sel();
                                break;
                        }
                        break;
                }

                switch (sensor.state) {
                case SENSOR_IDLE:
                        if (sensor.timer >= 500) {
                                sensor.timer = 0;
                                sensor.state = SENSOR_PWR_UP;

                                IO_SENSOR_PWR_SetHigh();
                        }
                        break;

                case SENSOR_PWR_UP:
                        if (sensor.timer >= 500) {
                                sensor.timer = 0;
                                sensor.state = SENSOR_READY;
                        }
                        break;

                case SENSOR_READY: {
                        sensor.state = SENSOR_IDLE;

                        if (power.sleep)
                                ++sensor.meas_cnt_slp;
                        else
                                ++sensor.meas_cnt;

                        sensor.last_adc = ADC_GetConversion(channel_AN2);

                        IO_SENSOR_PWR_SetLow();

                        if (sensor.last_adc < sensor.adc_min)
                                sensor.adc_min = sensor.last_adc;
                        if (sensor.last_adc > sensor.adc_max)
                                sensor.adc_max = sensor.last_adc;

                        if (sensor.warn) {
                                if (sensor.last_adc <= settings.adc_warn_off) {
                                        sensor.warn = 0;
                                }
                        } else {
                                if (sensor.last_adc >= settings.adc_warn) {
                                        sensor.warn = 1;
                                }
                        }

                        if (sensor.warn)
                                beep(250);
                }
                }

                if (display.pwr_on) {
                        switch (display.scr) {
                        case SCR_MAIN:
                                upd_scr_main();
                                break;

                        case SCR_MENU:
                                upd_scr_menu();
                                break;
                        }


                        if (display.timer >= 10000) {
                                display.pwr_on = 0;
                                ssd1306_send_command(SSD1306_CMD_DISPLAYOFF);

                                power.sleep = 1;
                        }
                }

                if (power.sleep && sensor.state == SENSOR_IDLE && !beeper.on) {
                        SLEEP();
                }
        }
}
/**
 End of File
 */