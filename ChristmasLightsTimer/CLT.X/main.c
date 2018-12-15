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


#define _XTAL_FREQ	31000


#define PIN_BUTTON	(GP5)
#define TRIS_BUTTON	(TRISIO5)
#define WPU_BUTTON	(WPU5)

#define PIN_LED		(GP2)
#define TRIS_LED	(TRISIO2)

#define TIMER2_PERIOD	(125)

volatile uint8_t timer2_cnt = 0;
volatile uint32_t elapsed_secs = 0;

volatile bit led_on = 0;
bit button_pressed = 0;

//#define TEST

#ifndef TEST
#define ON_PERIOD	(6 * 3600ul)
#define OFF_PERIOD	(18 * 3600ul)
#else
#define ON_PERIOD	(1)
#define OFF_PERIOD	(3)
#endif

void interrupt isr()
{
	if (TMR2IF) {
		TMR2IF = 0;
		++timer2_cnt;
		if (timer2_cnt >= TIMER2_PERIOD) {
			timer2_cnt = 0;
			++elapsed_secs;
		}
	}
}

void on_button_pressed()
{
	timer2_cnt = 0;
	elapsed_secs = 0;
	led_on = !led_on;
	PIN_LED = led_on;
}

void main()
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

	while (1) {
		if (led_on) {
			if (elapsed_secs >= ON_PERIOD) {
				elapsed_secs = 0;
				led_on = 0;
				PIN_LED = 0;
			}
		} else {
			if (elapsed_secs >= OFF_PERIOD) {
				elapsed_secs = 0;
				led_on = 1;
				PIN_LED = 1;
			}
		}

		if (PIN_BUTTON == 0 && !button_pressed) {
			on_button_pressed();
			button_pressed = 1;
		} else if (PIN_BUTTON == 1 && button_pressed) {
			button_pressed = 0;
		}
	}
}
