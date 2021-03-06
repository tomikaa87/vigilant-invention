/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using MPLAB(c) Code Configurator

  @Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 4.35
        Device            :  PIC16F1825
        Version           :  1.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

    Microchip licenses to you the right to use, modify, copy and distribute
    Software only when embedded on a Microchip microcontroller or digital signal
    controller that is integrated into your product or third party product
    (pursuant to the sublicense terms in the accompanying license agreement).

    You should refer to the license agreement accompanying this Software for
    additional information regarding your rights and obligations.

    SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
    EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
    MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
    IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
    CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
    OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
    INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
    CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
    SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
    (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

*/


#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set IO_OPTO_A aliases
#define IO_OPTO_A_TRIS               TRISAbits.TRISA0
#define IO_OPTO_A_LAT                LATAbits.LATA0
#define IO_OPTO_A_PORT               PORTAbits.RA0
#define IO_OPTO_A_WPU                WPUAbits.WPUA0
#define IO_OPTO_A_ANS                ANSELAbits.ANSA0
#define IO_OPTO_A_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define IO_OPTO_A_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define IO_OPTO_A_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define IO_OPTO_A_GetValue()           PORTAbits.RA0
#define IO_OPTO_A_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define IO_OPTO_A_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define IO_OPTO_A_SetPullup()      do { WPUAbits.WPUA0 = 1; } while(0)
#define IO_OPTO_A_ResetPullup()    do { WPUAbits.WPUA0 = 0; } while(0)
#define IO_OPTO_A_SetAnalogMode()  do { ANSELAbits.ANSA0 = 1; } while(0)
#define IO_OPTO_A_SetDigitalMode() do { ANSELAbits.ANSA0 = 0; } while(0)

// get/set IO_OPTO_B aliases
#define IO_OPTO_B_TRIS               TRISAbits.TRISA1
#define IO_OPTO_B_LAT                LATAbits.LATA1
#define IO_OPTO_B_PORT               PORTAbits.RA1
#define IO_OPTO_B_WPU                WPUAbits.WPUA1
#define IO_OPTO_B_ANS                ANSELAbits.ANSA1
#define IO_OPTO_B_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define IO_OPTO_B_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define IO_OPTO_B_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define IO_OPTO_B_GetValue()           PORTAbits.RA1
#define IO_OPTO_B_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define IO_OPTO_B_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define IO_OPTO_B_SetPullup()      do { WPUAbits.WPUA1 = 1; } while(0)
#define IO_OPTO_B_ResetPullup()    do { WPUAbits.WPUA1 = 0; } while(0)
#define IO_OPTO_B_SetAnalogMode()  do { ANSELAbits.ANSA1 = 1; } while(0)
#define IO_OPTO_B_SetDigitalMode() do { ANSELAbits.ANSA1 = 0; } while(0)

// get/set IO_OPTO_C aliases
#define IO_OPTO_C_TRIS               TRISAbits.TRISA2
#define IO_OPTO_C_LAT                LATAbits.LATA2
#define IO_OPTO_C_PORT               PORTAbits.RA2
#define IO_OPTO_C_WPU                WPUAbits.WPUA2
#define IO_OPTO_C_ANS                ANSELAbits.ANSA2
#define IO_OPTO_C_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define IO_OPTO_C_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define IO_OPTO_C_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define IO_OPTO_C_GetValue()           PORTAbits.RA2
#define IO_OPTO_C_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define IO_OPTO_C_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define IO_OPTO_C_SetPullup()      do { WPUAbits.WPUA2 = 1; } while(0)
#define IO_OPTO_C_ResetPullup()    do { WPUAbits.WPUA2 = 0; } while(0)
#define IO_OPTO_C_SetAnalogMode()  do { ANSELAbits.ANSA2 = 1; } while(0)
#define IO_OPTO_C_SetDigitalMode() do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set IO_OPTO_D aliases
#define IO_OPTO_D_TRIS               TRISAbits.TRISA4
#define IO_OPTO_D_LAT                LATAbits.LATA4
#define IO_OPTO_D_PORT               PORTAbits.RA4
#define IO_OPTO_D_WPU                WPUAbits.WPUA4
#define IO_OPTO_D_ANS                ANSELAbits.ANSA4
#define IO_OPTO_D_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define IO_OPTO_D_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define IO_OPTO_D_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define IO_OPTO_D_GetValue()           PORTAbits.RA4
#define IO_OPTO_D_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define IO_OPTO_D_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define IO_OPTO_D_SetPullup()      do { WPUAbits.WPUA4 = 1; } while(0)
#define IO_OPTO_D_ResetPullup()    do { WPUAbits.WPUA4 = 0; } while(0)
#define IO_OPTO_D_SetAnalogMode()  do { ANSELAbits.ANSA4 = 1; } while(0)
#define IO_OPTO_D_SetDigitalMode() do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set SCK aliases
#define SCK_TRIS               TRISCbits.TRISC0
#define SCK_LAT                LATCbits.LATC0
#define SCK_PORT               PORTCbits.RC0
#define SCK_WPU                WPUCbits.WPUC0
#define SCK_ANS                ANSELCbits.ANSC0
#define SCK_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define SCK_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define SCK_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define SCK_GetValue()           PORTCbits.RC0
#define SCK_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define SCK_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define SCK_SetPullup()      do { WPUCbits.WPUC0 = 1; } while(0)
#define SCK_ResetPullup()    do { WPUCbits.WPUC0 = 0; } while(0)
#define SCK_SetAnalogMode()  do { ANSELCbits.ANSC0 = 1; } while(0)
#define SCK_SetDigitalMode() do { ANSELCbits.ANSC0 = 0; } while(0)

// get/set SDI aliases
#define SDI_TRIS               TRISCbits.TRISC1
#define SDI_LAT                LATCbits.LATC1
#define SDI_PORT               PORTCbits.RC1
#define SDI_WPU                WPUCbits.WPUC1
#define SDI_ANS                ANSELCbits.ANSC1
#define SDI_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define SDI_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define SDI_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define SDI_GetValue()           PORTCbits.RC1
#define SDI_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define SDI_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define SDI_SetPullup()      do { WPUCbits.WPUC1 = 1; } while(0)
#define SDI_ResetPullup()    do { WPUCbits.WPUC1 = 0; } while(0)
#define SDI_SetAnalogMode()  do { ANSELCbits.ANSC1 = 1; } while(0)
#define SDI_SetDigitalMode() do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set SDO aliases
#define SDO_TRIS               TRISCbits.TRISC2
#define SDO_LAT                LATCbits.LATC2
#define SDO_PORT               PORTCbits.RC2
#define SDO_WPU                WPUCbits.WPUC2
#define SDO_ANS                ANSELCbits.ANSC2
#define SDO_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define SDO_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define SDO_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define SDO_GetValue()           PORTCbits.RC2
#define SDO_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define SDO_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define SDO_SetPullup()      do { WPUCbits.WPUC2 = 1; } while(0)
#define SDO_ResetPullup()    do { WPUCbits.WPUC2 = 0; } while(0)
#define SDO_SetAnalogMode()  do { ANSELCbits.ANSC2 = 1; } while(0)
#define SDO_SetDigitalMode() do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set IO_NRF_CE aliases
#define IO_NRF_CE_TRIS               TRISCbits.TRISC3
#define IO_NRF_CE_LAT                LATCbits.LATC3
#define IO_NRF_CE_PORT               PORTCbits.RC3
#define IO_NRF_CE_WPU                WPUCbits.WPUC3
#define IO_NRF_CE_ANS                ANSELCbits.ANSC3
#define IO_NRF_CE_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define IO_NRF_CE_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define IO_NRF_CE_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define IO_NRF_CE_GetValue()           PORTCbits.RC3
#define IO_NRF_CE_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define IO_NRF_CE_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define IO_NRF_CE_SetPullup()      do { WPUCbits.WPUC3 = 1; } while(0)
#define IO_NRF_CE_ResetPullup()    do { WPUCbits.WPUC3 = 0; } while(0)
#define IO_NRF_CE_SetAnalogMode()  do { ANSELCbits.ANSC3 = 1; } while(0)
#define IO_NRF_CE_SetDigitalMode() do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set IO_NRF_CSN aliases
#define IO_NRF_CSN_TRIS               TRISCbits.TRISC4
#define IO_NRF_CSN_LAT                LATCbits.LATC4
#define IO_NRF_CSN_PORT               PORTCbits.RC4
#define IO_NRF_CSN_WPU                WPUCbits.WPUC4
#define IO_NRF_CSN_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define IO_NRF_CSN_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define IO_NRF_CSN_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define IO_NRF_CSN_GetValue()           PORTCbits.RC4
#define IO_NRF_CSN_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define IO_NRF_CSN_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define IO_NRF_CSN_SetPullup()      do { WPUCbits.WPUC4 = 1; } while(0)
#define IO_NRF_CSN_ResetPullup()    do { WPUCbits.WPUC4 = 0; } while(0)

// get/set IO_NRF_IRQ aliases
#define IO_NRF_IRQ_TRIS               TRISCbits.TRISC5
#define IO_NRF_IRQ_LAT                LATCbits.LATC5
#define IO_NRF_IRQ_PORT               PORTCbits.RC5
#define IO_NRF_IRQ_WPU                WPUCbits.WPUC5
#define IO_NRF_IRQ_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define IO_NRF_IRQ_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define IO_NRF_IRQ_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define IO_NRF_IRQ_GetValue()           PORTCbits.RC5
#define IO_NRF_IRQ_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define IO_NRF_IRQ_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define IO_NRF_IRQ_SetPullup()      do { WPUCbits.WPUC5 = 1; } while(0)
#define IO_NRF_IRQ_ResetPullup()    do { WPUCbits.WPUC5 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/