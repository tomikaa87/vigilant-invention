/*
 * File:   main.c
 * Author: tomikaa
 *
 * Created on December 13, 2018, 6:36 PM
 */


#include <stdint.h>
#include <xc.h>


// CONFIG
#pragma config FOSC = INTOSCCLK // Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Detect (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)


#define _XTAL_FREQ	        31000


#define PIN_BUTTON	        (GP5)
#define TRIS_BUTTON	        (TRISIO5)
#define WPU_BUTTON	        (WPU5)

#define PIN_LED		        (GP2)
#define TRIS_LED	        (TRISIO2)

#define TIMER2_PERIOD	        (125)

#define CONFIG_ADDR             (0u)

#define LED_BLINK_OFF_TIME      (40u)
#define LED_BLINK_ON_TIME       (40u)
#define LED_BLINK_WAIT_TIME     (125u)

volatile uint8_t timer2_cnt = 0;
volatile uint32_t elapsed_secs = 0;

volatile bit led_on = 0;
bit button_pressed = 0;
bit button_long_pressed = 0;
uint32_t button_pressed_time = 0;

struct {
        uint16_t enabled : 1;
        uint16_t on : 1;
        uint16_t changed : 1;
        uint16_t count : 5;
        uint16_t current_count : 5;
        uint16_t : 0;
        uint16_t timer;
} led_blink;

enum {
        S_NORMAL,
        S_SETTING_ON_PERIOD
} state = S_NORMAL;

struct {
        uint8_t checksum;
        uint8_t on_hours;
} settings;

#define TEST

void led_blink_task();

void interrupt isr()
{
	if (TMR2IF) {
		TMR2IF = 0;
		++timer2_cnt;
		if (timer2_cnt >= TIMER2_PERIOD) {
			timer2_cnt = 0;
			++elapsed_secs;
		}

                led_blink_task();
	}
}

void setup()
{
        // 125 kHz HFINTOSC, FOSC defined clock source
	OSCCON = 0b00010000;

	// Shutdown the comparator to save energy
	CMCON0 = 0b111;

	// Setup input pin for the button
	TRIS_BUTTON = 1;
	OPTION_REGbits.nGPPU = 0;
	WPU_BUTTON = 1;

	// Setup output pin for the LED
	PIN_LED = 0;
	TRIS_LED = 0;

	// Setup TMR2, 8 ms period, Post-scaler 1:1, Enabled, Pre-scaler 1:1
	T2CON = 0b00000100;
	PR2 = 249;

	// Setup interrupts
	GIE = 1;
	PEIE = 1;
	TMR2IE = 1;

	led_on = 1;
	PIN_LED = 1;
}

uint8_t eeprom_read(const uint8_t address)
{
        EEADR = address;
        EECON1bits.RD = 1;
        return EEDAT;
}

void eeprom_write(const uint8_t address, const uint8_t data)
{
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
}

uint8_t settings_calc_cksum()
{
        const uint8_t* p = (const uint8_t*)&settings;
        uint8_t checksum = 0x10;

        // Skip 'checksum' field
        p += sizeof(settings.checksum);

        for (uint8_t i = 1; i < sizeof(settings); ++i) {
                checksum += *p++;
        }

        return checksum;
}

int8_t settings_load()
{
        uint8_t* p = (uint8_t*)&settings;

        for (uint8_t i = 0; i < sizeof(settings); ++i) {
                *p++ = eeprom_read(CONFIG_ADDR + i);
        }

        return settings_calc_cksum() == settings.checksum;
}

void settings_save()
{
        const uint8_t* p = (const uint8_t*)&settings;

        settings.checksum = settings_calc_cksum();

        for (uint8_t i = 0; i < sizeof(settings); ++i) {
                if (eeprom_read(CONFIG_ADDR +1) != *p)
                        eeprom_write(CONFIG_ADDR + i, *p++);
        }
}

void settings_load_defaults()
{
        settings.on_hours = 6u;
        settings.checksum = settings_calc_cksum();
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
        PIN_LED = 0;
        led_on = 0;
}

void normal_task()
{
#ifdef TEST
        static const uint32_t multiplier = 1u;
#else
        static const uint32_t multiplier = 3600u;
#endif

        if (led_on) {
                if (elapsed_secs >= settings.on_hours * multiplier) {
                        elapsed_secs = 0;
                        led_on = 0;
                        PIN_LED = 0;
                }
        } else {
                if (elapsed_secs >= (24u - settings.on_hours) * multiplier) {
                        elapsed_secs = 0;
                        led_on = 1;
                        PIN_LED = 1;
                }
        }
}

void set_on_period_task()
{
        if (!led_blink.enabled) {
                led_blink_start(settings.on_hours);
        }
}

void increment_on_period()
{
        ++settings.on_hours;

        if (settings.on_hours > 24) {
                settings.on_hours = 1;
        }

        led_blink_start(settings.on_hours);
}

void leave_on_period_setting()
{
        led_blink_stop();
        settings_save();

        state = S_NORMAL;
}

void led_blink_task()
{
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
                PIN_LED = led_blink.on;
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
                PIN_LED = 0;
                led_on = 0;
        }
}

void toggle_output()
{
        timer2_cnt = 0;
	elapsed_secs = 0;
	led_on = !led_on;
	PIN_LED = led_on;
}

void handle_short_button_press()
{
        switch (state) {
        case S_NORMAL:
                toggle_output();
                break;

        case S_SETTING_ON_PERIOD:
                increment_on_period();
                break;
        }
}

void handle_long_button_press()
{
        switch (state) {
        case S_NORMAL:
                state = S_SETTING_ON_PERIOD;
                break;

        case S_SETTING_ON_PERIOD:
                leave_on_period_setting();
                break;
        }
}

void check_button()
{
        if (PIN_BUTTON == 0) {
                if (!button_pressed) {
                        button_pressed = 1;
                        button_pressed_time = elapsed_secs;
                } else {
                        uint32_t elapsed = button_pressed_time - elapsed_secs;

                        if (elapsed >= 5u) {
                                button_long_pressed = 1;
                                handle_long_button_press();
                        }
                }
        } else {
                if (button_pressed) {
                        button_pressed = 0;
                        if (!button_long_pressed)
                                handle_short_button_press();
                        button_long_pressed = 0;
                }
        }
}

void main()
{
        setup();

        if (!settings_load()) {
                settings_load_defaults();
        }

	while (1) {
                switch (state) {
                case S_NORMAL:
                        normal_task();
                        break;

                case S_SETTING_ON_PERIOD:
                        set_on_period_task();
                        break;
                }

                check_button();
	}
}
