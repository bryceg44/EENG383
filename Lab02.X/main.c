//*****************************************************************
// Name:    Bryce Gelok and James Talbott
// Date:    Sep 10, 2020
// Lab:     02
//
// Purp:    Blink a morse code message
//
// Assisted by: Technical documents & Professor Coulston
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
#include <stdio.h>

// Configuration bits
#pragma config FOSC = INTIO67    // Oscillator Selection bits->Internal oscillator block
#pragma config PLLCFG = OFF    // 4X PLL Enable->Oscillator used directly
#pragma config PRICLKEN = ON    // Primary clock enable bit->Primary clock enabled

//definitions
#define TIME_UNIT               200
#define DOT_DELAY               1*TIME_UNIT
#define DASH_DELAY              3*TIME_UNIT

#define INTER_LETTER_DELAY      1*TIME_UNIT
#define INTRA_LETTER_DELAY      3*TIME_UNIT
#define INTRA_WORD_DELAY        7*TIME_UNIT

#define DOT                     0
#define DASH                    1
#define END                     2

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

//function prototypes
void initpic(void);
void microSecondDelay(uint16_t us);
void milliSecondDelay(uint16_t ms);
uint8_t convert(char letter);
void blink(char letter);

void main(void) {
    
    initpic();
   
    while(1==1) {
        while(BUTTON_PIN) {}
        char phrase[] = "sos";
        for(uint8_t i = 0; phrase[i] != '\0'; ++i) {
            blink(phrase[i]);
        }
    }
    return;
}

void microSecondDelay(uint16_t us) {
     for(uint16_t i = 0; i < us; ++i) {
         asm("NOP");
         asm("NOP");
         asm("NOP");
         asm("NOP");
         asm("NOP");
     }
}

void milliSecondDelay(uint16_t ms) {
    for (uint16_t i = 0; i < ms; ++i) {
         microSecondDelay(1000);
    }
}

uint8_t convert(char letter) {
    uint8_t num = letter;
    if (num < 97 || num > 122 ) {
        return 0;
    }
    return (num - 97);
}

void blink(char letter) {
    if (letter == ' ') {
        milliSecondDelay(INTRA_WORD_DELAY - INTRA_LETTER_DELAY);
        return;
    }
    
    uint8_t morseCode[26][5] = {
        	{DOT, DASH, END},               // a
        	{DASH, DOT, DOT, DOT, END},     // b
        	{DASH, DOT, DASH, DOT, END},    // c
            {DASH, DOT, DOT, END},               // d
            {DOT, END},                          // e
            {DOT, DOT, DASH, DOT, END},          // f
            {DASH, DASH, DOT, END},              // g
            {DOT, DOT, DOT, DOT, END},           // h
            {DOT, DOT, END},                     // i
            {DOT, DASH, DASH, DASH, END},        // j
            {DASH, DOT, DASH, END},              // k 
            {DOT, DASH, DOT, DOT, END},          // l
            {DASH, DASH, END},                   // m
            {DASH, DOT, END},                    // n
            {DASH, DASH, DASH, END},             // o
            {DOT, DASH, DASH, DOT, END},         // p
            {DASH, DASH, DOT, DASH, END},        // q
            {DOT, DASH, DOT, END},               // r
            {DOT, DOT, DOT, END},                // s
            {DASH, END},                         // t
            {DOT, DOT, DASH, END},               // u
            {DOT, DOT, DOT, DASH, END},          // v
            {DOT, DASH, DASH, END},              // w
            {DASH, DOT, DOT, DASH, END},         // x
            {DASH, DOT, DASH, DASH, END},        // y
            {DASH, DASH, DOT, DOT, END}          // z 
    }; 
	
    uint8_t index = convert(letter);
    
    for(uint8_t i = 0; i < 5; ++i) {
        uint8_t sign = morseCode[index][i];
        if(sign == END) {
            milliSecondDelay(INTRA_LETTER_DELAY - INTER_LETTER_DELAY);
            i = 5;
        }
        else {
            if(sign == DOT) {
                LED_PIN = 0;
                milliSecondDelay(DOT_DELAY);
                LED_PIN = 1;
                milliSecondDelay(INTER_LETTER_DELAY);
                continue;
            }
            else if (sign == DASH) {
                LED_PIN = 0;
                milliSecondDelay(DASH_DELAY);
                LED_PIN = 1;
                milliSecondDelay(INTER_LETTER_DELAY);
                continue;
            }
        }
    }
}

void initpic(void) {

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