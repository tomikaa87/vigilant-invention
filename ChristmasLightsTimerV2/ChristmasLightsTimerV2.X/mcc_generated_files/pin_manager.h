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
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC16LF15313
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.31 and above
        MPLAB 	          :  MPLAB X 5.45	
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

// get/set RA2 procedures
#define RA2_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define RA2_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define RA2_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define RA2_GetValue()              PORTAbits.RA2
#define RA2_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define RA2_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define RA2_SetPullup()             do { WPUAbits.WPUA2 = 1; } while(0)
#define RA2_ResetPullup()           do { WPUAbits.WPUA2 = 0; } while(0)
#define RA2_SetAnalogMode()         do { ANSELAbits.ANSA2 = 1; } while(0)
#define RA2_SetDigitalMode()        do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set IO_CLK aliases
#define IO_CLK_TRIS                 TRISAbits.TRISA4
#define IO_CLK_LAT                  LATAbits.LATA4
#define IO_CLK_PORT                 PORTAbits.RA4
#define IO_CLK_WPU                  WPUAbits.WPUA4
#define IO_CLK_OD                   ODCONAbits.ODCA4
#define IO_CLK_ANS                  ANSELAbits.ANSA4
#define IO_CLK_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define IO_CLK_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define IO_CLK_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define IO_CLK_GetValue()           PORTAbits.RA4
#define IO_CLK_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define IO_CLK_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define IO_CLK_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define IO_CLK_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define IO_CLK_SetPushPull()        do { ODCONAbits.ODCA4 = 0; } while(0)
#define IO_CLK_SetOpenDrain()       do { ODCONAbits.ODCA4 = 1; } while(0)
#define IO_CLK_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define IO_CLK_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set IO_BTN aliases
#define IO_BTN_TRIS                 TRISAbits.TRISA5
#define IO_BTN_LAT                  LATAbits.LATA5
#define IO_BTN_PORT                 PORTAbits.RA5
#define IO_BTN_WPU                  WPUAbits.WPUA5
#define IO_BTN_OD                   ODCONAbits.ODCA5
#define IO_BTN_ANS                  ANSELAbits.ANSA5
#define IO_BTN_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define IO_BTN_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define IO_BTN_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define IO_BTN_GetValue()           PORTAbits.RA5
#define IO_BTN_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define IO_BTN_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define IO_BTN_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define IO_BTN_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)
#define IO_BTN_SetPushPull()        do { ODCONAbits.ODCA5 = 0; } while(0)
#define IO_BTN_SetOpenDrain()       do { ODCONAbits.ODCA5 = 1; } while(0)
#define IO_BTN_SetAnalogMode()      do { ANSELAbits.ANSA5 = 1; } while(0)
#define IO_BTN_SetDigitalMode()     do { ANSELAbits.ANSA5 = 0; } while(0)

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