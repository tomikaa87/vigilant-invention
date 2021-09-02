/*
 * File:   main.c
 * Author: tomikaa
 *
 * Created on December 13, 2018, 6:36 PM
 */

/*
 * Menu representation with a single LED
 *
 * Normal state: LED switched by the main timer
 *  |--> Long press: enter, leave
 *     |--> LED blink (1x 1000 ms + 1x 500 ms + 1000 ms off): 1st option - ON time
 *     |  |--> Long press: enter, leave; Short press: increment value
 *     |  |--> LED blink (Nx 500 ms + 1000 ms off): N hours ON
 *     |--> LED blink (1x 1000 ms + 2x 500 ms + 1000 ms off): 2nd option - LED brightness
 *        |--> Long press: enter, leave; Short press: increment value
 *        |--> Change LED brightness in 10% steps
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <xc.h>

#ifndef _12F683
#include "mcc_generated_files/mcc.h"
#endif


// CONFIG
#ifdef _12F683
#pragma config FOSC = INTOSCCLK // Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Detect (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

#define _XTAL_FREQ              1000000
#endif

#ifdef _12F683
#define PIN_BUTTON              (GP5)
#define TRIS_BUTTON             (TRISIO5)
#define WPU_BUTTON              (WPU5)

#define PIN_LED                 (GP2)
#define TRIS_LED                (TRISIO2)

#define TIMER2_PERIOD           (125)

#define LED_BLINK_OFF_TIME      (40u)
#define LED_BLINK_ON_TIME       (40u)
#define LED_BLINK_WAIT_TIME     (125u)

#define TIMER_TICKS_PER_SEC     (100)
#else
#define PIN_BUTTON              (IO_BUTTON_GetValue())
#define TIMER_TICKS_PER_SEC     (100)

#define LED_BLINK_OFF_TIME      (40u)
#define LED_BLINK_ON_TIME       (40u)
#define LED_BLINK_WAIT_TIME     (125u)

#endif

#define CONFIG_ADDR             (0u)
#define TIMER_PR2_ADDR          (0xFD)
#define OSCTUNE_ADDR            (0xFE)
#define OSCTUNE_STORED_ADDR     (0xFF)

// For my 12F683, these tuning values are needed:
//  PR2 = 250
//  OSCTUNE = 0
//
//  EEPROM bytes [0xFD..0xFF] = 0xFA 0x00 0xAB


#define ENABLE_TEST_MODE        (0)


#define WRITE_OSCTUNE_TO_EEPROM (0)
#define OSCTUNE_VALUE           (0b11111)


volatile uint8_t timer_cnt = 0;
volatile uint32_t elapsed_secs = 0;

volatile bool led_on = 0;

struct {
        uint8_t pressed : 1;
        uint8_t long_pressed : 1;
        uint8_t debounce_counter : 6;
        uint8_t press_time;
} button = { 0 };

struct {
        uint8_t run_task : 1;
        uint8_t enabled : 1;
        uint8_t on : 1;
        uint8_t changed : 1;
        uint8_t count : 5;
        uint8_t current_count : 5;
        uint8_t: 0;
        uint8_t timer;
} led_blink = { 0 };

enum {
        S_TIMER,
        S_MENU,
        S_SET_ON_HOURS,
        S_SET_BRIGHTNESS
} state = S_TIMER;

struct {
        uint8_t checksum;
        uint8_t on_hours;
        uint8_t brightness_pct;
} settings;


void led_blink_task();

#ifdef _12F683


void __interrupt() isr()
{
        if (TMR2IF) {
                TMR2IF = 0;
                ++timer_cnt;
                if (timer_cnt >= TIMER2_PERIOD) {
                        timer_cnt = 0;
                        ++elapsed_secs;
                        ++button.press_time;
                }

                led_blink.run_task = 1;
        }
}
#endif


void setup()
{
#ifdef _12F683
        // 1 Mhz HFINTOSC, FOSC defined clock source
        OSCCON = 0b01000000;

        // Shutdown the comparator to save energy
        CMCON0 = 0b111;

        // Setup input pin for the button
        TRIS_BUTTON = 1;
        OPTION_REGbits.nGPPU = 0;
        WPU_BUTTON = 1;

        // Setup output pin for the LED
        PIN_LED = 0;
        TRIS_LED = 0;

        // Setup TMR2, 8 ms period, Post-scaler 1:8, Enabled, Pre-scaler 1:1
        T2CON = 0x3C;
        PR2 = 249;

        // Setup interrupts
        GIE = 1;
        PEIE = 1;
        TMR2IE = 1;

        led_on = 1;
        PIN_LED = 1;
#endif

#if WRITE_OSCTUNE_TO_EEPROM
        eeprom_write(OSCTUNE_STORED_ADDR, 0xAB);
        eeprom_write(OSCTUNE_ADDR, OSCTUNE_VALUE);
#endif

        // Apply custom oscillator tuning value if needed
        if (eeprom_read(OSCTUNE_STORED_ADDR) == 0xAB) {
                uint8_t stored_osctune = eeprom_read(OSCTUNE_ADDR);
                if (stored_osctune <= 0b11111) {
                        OSCTUNE = stored_osctune;
                }

                PR2 = eeprom_read(TIMER_PR2_ADDR);
        }
}


uint8_t eeprom_read(const uint8_t address)
{
#ifdef _12F683
        EEADR = address;
        EECON1bits.RD = 1;
        return EEDAT;
#else
        return EEPROM_READ(address);
#endif
}


void eeprom_write(const uint8_t address, const uint8_t data)
{
#ifdef _12F683
        EECON1bits.WREN = 1;
        EEADR = address;
        EEDAT = data;
        GIE = 0;
        EECON2 = 0x55;
        EECON2 = 0xAA;
        EECON1bits.WR = 1;
        while (EECON1bits.WR);
        EECON1bits.WREN = 0;
        GIE = 1;
#else
        EEPROM_WRITE(address, data);
#endif
}


uint8_t settings_calc_checksum()
{
        const uint8_t* p = (const uint8_t*)&settings;
        uint8_t checksum = 0x10;

        // Skip 'checksum' field
        p += sizeof (settings.checksum);

        for (uint8_t i = sizeof (settings.checksum); i < sizeof (settings); ++i) {
                checksum += *p++;
        }

        return checksum;
}


int8_t settings_load()
{
        uint8_t* p = (uint8_t*)&settings;

        for (uint8_t i = 0; i < sizeof (settings); ++i) {
                *p++ = eeprom_read(CONFIG_ADDR + i);
        }

        return settings_calc_checksum() == settings.checksum;
}


void settings_save()
{
        const uint8_t* p = (const uint8_t*)&settings;

        settings.checksum = settings_calc_checksum();

        for (uint8_t i = 0; i < sizeof (settings); ++i) {
                if (eeprom_read(CONFIG_ADDR + i) != *p) {
                        eeprom_write(CONFIG_ADDR + i, *p++);
                }
        }
}


void settings_load_defaults()
{
        settings.on_hours = 6u;
        settings.brightness_pct = 100u;
        settings.checksum = settings_calc_checksum();
}


void led_set_on()
{
#ifdef _12F683
        PIN_LED = 1;
#else
        EPWM_LoadDutyValue(1023);
#endif
}


void led_set_off()
{
#ifdef _12F683
        PIN_LED = 0;
#else
        EPWM_LoadDutyValue(0);
#endif
}


void led_blink_start(uint8_t count)
{
        led_blink.count = count;
        led_blink.current_count = 0;
        led_blink.timer = 0;
        led_blink.on = 0;
        led_blink.changed = 0;
        led_blink.enabled = 1;
}


void led_blink_stop()
{
        led_blink.enabled = 0;
        led_set_off();
        led_on = 0;
}


void timer_task()
{
#if ENABLE_TEST_MODE
        static const uint32_t multiplier = 1u;
#else
        static const uint32_t multiplier = 3600u;
#endif

        if (settings.on_hours == 24) {
                if (!led_on) {
                        led_on = 1;
                        led_set_on();
                }

                return;
        }

        if (led_on) {
                if (elapsed_secs >= settings.on_hours * multiplier) {
                        elapsed_secs = 0;
                        led_on = 0;
                        led_set_off();
                }
        } else {
                if (elapsed_secs >= (24u - settings.on_hours) * multiplier) {
                        elapsed_secs = 0;
                        led_on = 1;
                        led_set_on();
                }
        }
}


void enter_on_hours_setting()
{
        if (state == S_TIMER) {
                state = S_SET_ON_HOURS;
                led_blink_start(settings.on_hours);
        }
}


void increment_on_hours_setting()
{
        ++settings.on_hours;

        if (settings.on_hours > 24) {
                settings.on_hours = 1;
        }

        led_blink_start(settings.on_hours);
}


void leave_on_hours_setting()
{
        led_blink_stop();
        settings_save();

        state = S_TIMER;
}


void led_blink_task()
{
        led_blink.run_task = 0;

        if (!led_blink.enabled)
                return;

        // Wait until the set time elapses
        if (led_blink.timer > 0) {
                --led_blink.timer;
                return;
        }

        // Change the state of the LED if needed
        if (led_blink.changed) {
                led_blink.changed = 0;
                if (led_blink.on) {
                        led_set_on();
                } else {
                        led_set_off();
                }
                led_on = led_blink.on;
        }

        if (led_blink.on) {
                // The LED is on, wait for ON time and then turn it off
                led_blink.on = 0;
                led_blink.changed = 1;
                led_blink.timer = LED_BLINK_ON_TIME;
        } else if (led_blink.current_count > 1) {
                // The LED off, wait for OFF time and then turn it on
                led_blink.on = 1;
                led_blink.changed = 1;
                led_blink.timer = LED_BLINK_OFF_TIME;
                --led_blink.current_count;
        } else {
                // No more counts, wait for WAIT time and restart the counter.
                // After that, turn the LED on.
                led_blink.timer = LED_BLINK_WAIT_TIME;
                led_blink.current_count = led_blink.count;
                led_blink.on = 1;
                led_blink.changed = 1;
                led_set_off();
                led_on = 0;
        }
}


void switch_led()
{
        timer_cnt = 0;
        elapsed_secs = 0;
        led_on = (bool)!led_on;

        if (led_on) {
                led_set_on();
        } else {
                led_set_off();
        }
}


void handle_short_button_press()
{
        switch (state) {
        case S_TIMER:
                switch_led();
                break;

        case S_SET_ON_HOURS:
                increment_on_hours_setting();
                break;
        }
}


void handle_long_button_press()
{
        switch (state) {
        case S_TIMER:
                enter_on_hours_setting();
                break;

        case S_SET_ON_HOURS:
                leave_on_hours_setting();
                break;
        }
}


void check_button()
{
        if (--button.debounce_counter > 0)
                return;

        button.debounce_counter = 10;

        if (PIN_BUTTON == 0) {
                if (!button.pressed) {
                        button.pressed = 1;
                        button.press_time = 0;
                } else {
                        if (button.long_pressed)
                                return;

                        if (button.press_time >= 5u) {
                                button.long_pressed = 1;
                                handle_long_button_press();
                        }
                }
        } else {
                if (button.pressed) {
                        button.pressed = 0;
                        if (!button.long_pressed)
                                handle_short_button_press();
                        button.long_pressed = 0;
                }
        }
}

#ifndef _12F683


void timer0_isr()
{
        if (++timer_cnt >= TIMER_TICKS_PER_SEC) {
                timer_cnt = 0;
                ++elapsed_secs;
                ++button.press_time;
        }

        led_blink.run_task = 1;
}
#endif


void run()
{
        while (1) {
                if (state == S_TIMER) {
                        timer_task();
                }

                if (led_blink.run_task) {
                        led_blink_task();
                }

                check_button();
        }
}


void main()
{
#ifndef _12F683
        SYSTEM_Initialize();
        TMR2_StartTimer();
        TMR0_SetInterruptHandler(timer0_isr);
        INTERRUPT_GlobalInterruptEnable();
        INTERRUPT_PeripheralInterruptEnable();
#endif

        led_set_on();

        setup();

        if (!settings_load()) {
                settings_load_defaults();
        }

        run();
}