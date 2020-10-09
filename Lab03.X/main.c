//*****************************************************************
// Name:    Bryce Gelok and James Talbott
// Date:    Fall 2020
// Lab:     03
// Purp: Play O Canada
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
#include "mcc_generated_files/mcc.h"


#define SONG_LENGTH     21       // Number of notes
#define NUM_OCTAVES     2

#define QUA     15625   // 0.25 seconds
#define HAL     (QUA * 2)  
#define WHO     (QUA * 4)  
#define EIG     (QUA / 2)  
#define SIX     (QUA / 4)
#define THIR    (QUA / 8)
#define DOTTEDQUA  (QUA + EIG)
#define DOTTEDHAL  (HAL + QUA)

#define REST    25  // rest index to trigger if statement

//note index definitions for scale array
#define A4      0
#define AS4     1
#define BF4     1
#define B4      2
#define C5      3
#define CS5     4
#define DF5     4
#define D5      5
#define DS5     6
#define EF5     6
#define E5      7
#define F5      8
#define FS5     9
#define GF5     9
#define G5      10
#define GS5     11
#define AF5     11
#define A5      12
#define AS5     13
#define BF5     13
#define B5      14
#define C6      15
#define CS6     16
#define DF6     16
#define D6      17
#define DS6     18
#define EF6     18
#define E6      19
#define F6      20
#define FS6     21
#define GF6     21
#define G6      22
#define GS6     23
#define AF6     23
#define A6      24

// two octaves worth of notes
uint16_t scale[NUM_OCTAVES*12] = {18182,17167,16194,15296,14440,13629,12862,12140,
                                  11461,10811,10204,9627,9091,8584,8105,7648,7220,
                                  6814,6431,6070,5731,5409,5105,4819,4548};


void main(void) {
    
    //O Canada 
    uint16_t notes[SONG_LENGTH] = {A5, C6, C6, F5, G5, A5, BF5, C6, D6, G5, REST, A5, B5, B5, C6, D6, E6, E6, D6, D6, C6}; // microseconds
    uint16_t duration[SONG_LENGTH] = {HAL, DOTTEDQUA, EIG, DOTTEDHAL, QUA, QUA, QUA, QUA, QUA, DOTTEDHAL, QUA, HAL, DOTTEDQUA, EIG, DOTTEDHAL, QUA, QUA, QUA,QUA,QUA, DOTTEDHAL}; // 1:1 timer counts
    
    SYSTEM_Initialize();
    
    for (;;) {
        //  wait for button press
        while(TOP_BUTTON_GetValue() == HIGH);
        
        for(uint8_t i = 0; i < SONG_LENGTH; ++i) {  
            TMR0_Initialize();
            TMR0_WriteTimer(0x10000 - duration[i]);
            while (TMR0_HasOverflowOccured() == false) {
                if(notes[i] != 25) {//  checks if rest index
                    TMR1_Initialize();
                    TMR1_WriteTimer(0x10000 - scale[notes[i]]);
                    while (TMR1_HasOverflowOccured() == false);
                    SPEAKER_PIN_Toggle();
                } // end TMR1 loop - tone
            } // end TMR0 loop - duration
            
            //  pause between notes
            SPEAKER_PIN_SetLow();
            TMR0_Initialize();
            TMR0_WriteTimer(0x10000 - THIR);
            while (TMR0_HasOverflowOccured() == false);
        }
    }// end infinite loop       
}// end main   

