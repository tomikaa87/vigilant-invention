/*
 * File:   main.c
 * Author: tomikaa
 *
 * Created on 2018. március 18., 19:00
 */

#include <xc.h>

#include "mcc_generated_files/mcc.h"
#include "opto.h"
#include "radio.h"

void main()
{
	SYSTEM_Initialize();
	radio_init();
	
#ifdef TEST_OPTO
	uint8_t index = 0;
	
	while (true) {
		if (index < 8) {
			opto_enable(index);
			++index;
		} else {
			opto_disable();
			index = 0;
		}
		
		for (uint8_t i = 0; i < 200; ++i) {
			CLRWDT();
			__delay_ms(10);
		}
	}
#else
	while (true);
#endif
}
