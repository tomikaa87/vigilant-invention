#include "opto.h"
#include "mcc_generated_files/pin_manager.h"

#include <xc.h>

void opto_enable(uint8_t index)
{
	opto_disable();
	
	switch (index)
	{
	case 0:
		IO_OPTO_A_LAT = 1;
		IO_OPTO_B_LAT = 0;
		IO_OPTO_A_TRIS = 0;
		IO_OPTO_B_TRIS = 0;
		break;
		
	case 1:
		IO_OPTO_A_LAT = 0;
		IO_OPTO_B_LAT = 1;
		IO_OPTO_A_TRIS = 0;
		IO_OPTO_B_TRIS = 0;
		break;
		
	case 2:
		IO_OPTO_B_LAT = 1;
		IO_OPTO_C_LAT = 0;
		IO_OPTO_B_TRIS = 0;
		IO_OPTO_C_TRIS = 0;
		break;
		
	case 3:
		IO_OPTO_B_LAT = 0;
		IO_OPTO_C_LAT = 1;
		IO_OPTO_B_TRIS = 0;
		IO_OPTO_C_TRIS = 0;
		break;
		
	case 4:
		IO_OPTO_C_LAT = 0;
		IO_OPTO_D_LAT = 1;
		IO_OPTO_C_TRIS = 0;
		IO_OPTO_D_TRIS = 0;
		break;
		
	case 5:
		IO_OPTO_C_LAT = 1;
		IO_OPTO_D_LAT = 0;
		IO_OPTO_C_TRIS = 0;
		IO_OPTO_D_TRIS = 0;
		break;
		
	case 6:
		IO_OPTO_A_LAT = 1;
		IO_OPTO_C_LAT = 0;
		IO_OPTO_A_TRIS = 0;
		IO_OPTO_C_TRIS = 0;
		break;
		
	case 7:
		IO_OPTO_A_LAT = 0;
		IO_OPTO_C_LAT = 1;
		IO_OPTO_A_TRIS = 0;
		IO_OPTO_C_TRIS = 0;
		break;
		
	default:
		break;
	}
}

void opto_disable()
{
	IO_OPTO_A_TRIS = 1;
	IO_OPTO_B_TRIS = 1;
	IO_OPTO_C_TRIS = 1;
	IO_OPTO_D_TRIS = 1;
}