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
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.5
        Device            :  PIC18F26K22
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.20 and above
        MPLAB 	          :  MPLAB X 5.40	
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

// get/set BUTTON_PIN aliases
#define BUTTON_PIN_TRIS                 TRISAbits.TRISA2
#define BUTTON_PIN_LAT                  LATAbits.LATA2
#define BUTTON_PIN_PORT                 PORTAbits.RA2
#define BUTTON_PIN_ANS                  ANSELAbits.ANSA2
#define BUTTON_PIN_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define BUTTON_PIN_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define BUTTON_PIN_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define BUTTON_PIN_GetValue()           PORTAbits.RA2
#define BUTTON_PIN_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define BUTTON_PIN_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define BUTTON_PIN_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define BUTTON_PIN_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set SPEAKER_PIN aliases
#define SPEAKER_PIN_TRIS                 TRISBbits.TRISB5
#define SPEAKER_PIN_LAT                  LATBbits.LATB5
#define SPEAKER_PIN_PORT                 PORTBbits.RB5
#define SPEAKER_PIN_WPU                  WPUBbits.WPUB5
#define SPEAKER_PIN_ANS                  ANSELBbits.ANSB5
#define SPEAKER_PIN_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define SPEAKER_PIN_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define SPEAKER_PIN_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define SPEAKER_PIN_GetValue()           PORTBbits.RB5
#define SPEAKER_PIN_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define SPEAKER_PIN_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define SPEAKER_PIN_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define SPEAKER_PIN_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define SPEAKER_PIN_SetAnalogMode()      do { ANSELBbits.ANSB5 = 1; } while(0)
#define SPEAKER_PIN_SetDigitalMode()     do { ANSELBbits.ANSB5 = 0; } while(0)

// get/set RC6 procedures
#define RC6_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define RC6_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define RC6_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define RC6_GetValue()              PORTCbits.RC6
#define RC6_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define RC6_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define RC6_SetAnalogMode()         do { ANSELCbits.ANSC6 = 1; } while(0)
#define RC6_SetDigitalMode()        do { ANSELCbits.ANSC6 = 0; } while(0)

// get/set RC7 procedures
#define RC7_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define RC7_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define RC7_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RC7_GetValue()              PORTCbits.RC7
#define RC7_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define RC7_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define RC7_SetAnalogMode()         do { ANSELCbits.ANSC7 = 1; } while(0)
#define RC7_SetDigitalMode()        do { ANSELCbits.ANSC7 = 0; } while(0)

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