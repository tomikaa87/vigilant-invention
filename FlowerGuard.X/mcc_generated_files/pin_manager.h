/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC16F1825
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05 and above
        MPLAB 	          :  MPLAB X 5.20	
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

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set channel_AN2 aliases
#define channel_AN2_TRIS                 TRISAbits.TRISA2
#define channel_AN2_LAT                  LATAbits.LATA2
#define channel_AN2_PORT                 PORTAbits.RA2
#define channel_AN2_WPU                  WPUAbits.WPUA2
#define channel_AN2_ANS                  ANSELAbits.ANSA2
#define channel_AN2_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define channel_AN2_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define channel_AN2_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define channel_AN2_GetValue()           PORTAbits.RA2
#define channel_AN2_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define channel_AN2_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define channel_AN2_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define channel_AN2_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define channel_AN2_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define channel_AN2_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set IO_SENSOR_PWR aliases
#define IO_SENSOR_PWR_TRIS                 TRISAbits.TRISA4
#define IO_SENSOR_PWR_LAT                  LATAbits.LATA4
#define IO_SENSOR_PWR_PORT                 PORTAbits.RA4
#define IO_SENSOR_PWR_WPU                  WPUAbits.WPUA4
#define IO_SENSOR_PWR_ANS                  ANSELAbits.ANSA4
#define IO_SENSOR_PWR_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define IO_SENSOR_PWR_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define IO_SENSOR_PWR_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define IO_SENSOR_PWR_GetValue()           PORTAbits.RA4
#define IO_SENSOR_PWR_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define IO_SENSOR_PWR_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define IO_SENSOR_PWR_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define IO_SENSOR_PWR_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define IO_SENSOR_PWR_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define IO_SENSOR_PWR_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set IO_BUTTON1 aliases
#define IO_BUTTON1_TRIS                 TRISAbits.TRISA5
#define IO_BUTTON1_LAT                  LATAbits.LATA5
#define IO_BUTTON1_PORT                 PORTAbits.RA5
#define IO_BUTTON1_WPU                  WPUAbits.WPUA5
#define IO_BUTTON1_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define IO_BUTTON1_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define IO_BUTTON1_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define IO_BUTTON1_GetValue()           PORTAbits.RA5
#define IO_BUTTON1_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define IO_BUTTON1_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define IO_BUTTON1_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define IO_BUTTON1_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)

// get/set SCL aliases
#define SCL_TRIS                 TRISCbits.TRISC0
#define SCL_LAT                  LATCbits.LATC0
#define SCL_PORT                 PORTCbits.RC0
#define SCL_WPU                  WPUCbits.WPUC0
#define SCL_ANS                  ANSELCbits.ANSC0
#define SCL_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define SCL_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define SCL_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define SCL_GetValue()           PORTCbits.RC0
#define SCL_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define SCL_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define SCL_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define SCL_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define SCL_SetAnalogMode()      do { ANSELCbits.ANSC0 = 1; } while(0)
#define SCL_SetDigitalMode()     do { ANSELCbits.ANSC0 = 0; } while(0)

// get/set SDA aliases
#define SDA_TRIS                 TRISCbits.TRISC1
#define SDA_LAT                  LATCbits.LATC1
#define SDA_PORT                 PORTCbits.RC1
#define SDA_WPU                  WPUCbits.WPUC1
#define SDA_ANS                  ANSELCbits.ANSC1
#define SDA_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define SDA_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define SDA_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define SDA_GetValue()           PORTCbits.RC1
#define SDA_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define SDA_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define SDA_SetPullup()          do { WPUCbits.WPUC1 = 1; } while(0)
#define SDA_ResetPullup()        do { WPUCbits.WPUC1 = 0; } while(0)
#define SDA_SetAnalogMode()      do { ANSELCbits.ANSC1 = 1; } while(0)
#define SDA_SetDigitalMode()     do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set SDO aliases
#define SDO_TRIS                 TRISCbits.TRISC2
#define SDO_LAT                  LATCbits.LATC2
#define SDO_PORT                 PORTCbits.RC2
#define SDO_WPU                  WPUCbits.WPUC2
#define SDO_ANS                  ANSELCbits.ANSC2
#define SDO_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define SDO_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define SDO_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define SDO_GetValue()           PORTCbits.RC2
#define SDO_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define SDO_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define SDO_SetPullup()          do { WPUCbits.WPUC2 = 1; } while(0)
#define SDO_ResetPullup()        do { WPUCbits.WPUC2 = 0; } while(0)
#define SDO_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define SDO_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set RC5 procedures
#define RC5_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define RC5_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define RC5_GetValue()              PORTCbits.RC5
#define RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define RC5_SetPullup()             do { WPUCbits.WPUC5 = 1; } while(0)
#define RC5_ResetPullup()           do { WPUCbits.WPUC5 = 0; } while(0)

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


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCAF5 pin functionality
 * @Example
    IOCAF5_ISR();
 */
void IOCAF5_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCAF5 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCAF5 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF5_SetInterruptHandler(MyInterruptHandler);

*/
void IOCAF5_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCAF5 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCAF5_SetInterruptHandler() method.
    This handler is called every time the IOCAF5 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF5_SetInterruptHandler(IOCAF5_InterruptHandler);

*/
extern void (*IOCAF5_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCAF5 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCAF5_SetInterruptHandler() method.
    This handler is called every time the IOCAF5 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF5_SetInterruptHandler(IOCAF5_DefaultInterruptHandler);

*/
void IOCAF5_DefaultInterruptHandler(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/