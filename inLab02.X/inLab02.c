//*****************************************************************
// Name:    Dr. Chris Coulston
// Date:    Aug 31, 2017
// Lab:     02
//
// Purp:    Setting up a button and LED
//
// Assisted: The entire EENG 383 class
// Assisted by: Technical documents
//
// Academic Integrity Statement: I certify that, while others may have
// assisted me in brain storming, debugging and validating this program,
// the program itself is my own work. I understand that submitting code
// which is the work of other individuals is a violation of the course
// Academic Integrity Policy and may result in a zero credit for the
// assignment, course failure and a report to the Academic Dishonesty
// Board. I also understand that if I knowingly give my original work to
// another individual that it could also result in a zero credit for the
// assignment, course failure and a report to the Academic Dishonesty
// Board.
//*****************************************************************
#include <xc.h>
#include <stdint.h>             // look @ \Microchip\xc8\v1.38\include
#include <stdbool.h>

// Configuration bits
#pragma config FOSC = INTIO67    // Oscillator Selection bits->Internal oscillator block
#pragma config PLLCFG = OFF    // 4X PLL Enable->Oscillator used directly
#pragma config PRICLKEN = ON    // Primary clock enable bit->Primary clock enabled

void initPIC(void);
void milliSecondDelay(uint16_t ms);
void microSecondDelay(uint16_t us);

#define BUTTON_PIN  PORTAbits.RA2       // always use "PORT" for inputs
#define BUTTON_TRIS TRISAbits.TRISA2    // Make your code self-documenting
#define BUTTON_ANG  ANSELAbits.ANSA2    // needed for digital input

#define LED_PIN     LATBbits.LATB5      // always use "LAT" for outputs
#define LED_TRIS    TRISBbits.TRISB5
#define LED_ANG     ANSELBbits.ANSB5

#define INPUT       1                   // Make code self-documenting
#define OUTPUT      0                   // Page 135 of PIC18(L)F2X/4XK22 Data Sheet

#define ANALOG      1                   // Page PIC18(L)F2X/4XK22 Data Sheet
#define DIGITAL     0

//*****************************************************************

//*****************************************************************

void main(void) {

    initPIC();

    for (;;) {

        LED_PIN = 0;

        while (BUTTON_PIN == 1); // While button is not pressed, wait
        while (BUTTON_PIN = 0); // The button is being held down

        // Toggle the LED at 1Hz while the button is not being pressed
        while (BUTTON_PIN == 1) {
            LED_PIN ^= 1;
            milliSecondDelay(1);
        } // end 


    } // end infinite loop

} // end main


//*****************************************************************
// Initialize initPIC
//*****************************************************************

void initPIC(void) {

    // ---------------Configure Oscillator------------------
    OSCCONbits.IRCF2 = 1; // Internal RC Oscillator Frequency Select bits
    OSCCONbits.IRCF1 = 1; // Set to 16Mhz
    OSCCONbits.IRCF0 = 1; //
    OSCTUNEbits.PLLEN = 1; // enable the 4xPLL, wicked fast 64Mhz

    BUTTON_ANG = DIGITAL; // Must do for any input which is multiplex with ADC
    BUTTON_TRIS = INPUT; // initialize DDR bit makes push button an input

    LED_ANG = DIGITAL; // Not really needed because LED is an output
    LED_TRIS = OUTPUT; // initialize DDR bit makes LED an output

} // end initPIC


//*****************************************************************
// Call microseconds(1000) a number of times
//*****************************************************************

void milliSecondDelay(uint16_t ms) {

    uint16_t i;

    for (i = 0; i < ms; i++) microSecondDelay(1000);

} // end millSecond


//*****************************************************************
// Create a delay of 1uS and loop a number of times
//*****************************************************************

void microSecondDelay(uint16_t us) {

    uint16_t i;

    for (i = 0; i < us; i++) {
        asm("NOP"); // 1
        asm("NOP"); // 2
        asm("NOP"); // 3
        asm("NOP"); // 4

        i = i;
    } // end for     
} // end microSecond()