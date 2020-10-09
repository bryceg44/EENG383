//*****************************************************************
// Name:Bryce Gelok and James Talbott
// Date:	Fall 2020
// Lab: 	05
// Purp:	Use a terminal interface to have a color show
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
#include <inttypes.h>
#pragma warning disable 520     // warning: (520) function "xyz" is never called  
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

#define NUM_COLOR 6
#define DUTY_INC 
uint8_t colorTour = false;

void main(void) {
    //                             Yel    Red  Fuc   Blu   Tur   Gre
    uint16_t initRed[NUM_COLOR] = {0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF};
    uint16_t initGre[NUM_COLOR] = {0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00};
    uint16_t initBlu[NUM_COLOR] = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF};
    
    uint16_t deltaRed[NUM_COLOR] = {0x0000, 0x0000, 0x0001, 0x0000, 0x0000};
    uint16_t deltaGre[NUM_COLOR] = {0x0001, 0x0000, 0x0000, 0xFFFF, 0x0000};
    uint16_t deltaBlu[NUM_COLOR] = {0x0000, 0xFFFF, 0x0000, 0x0000, 0x0001};
    

    
    uint16_t duty = 127;
    uint8_t i;
    char cmd;
    
    

    SYSTEM_Initialize();

    // Not necessary, but this delay allows the baud rate generator to 
    // stablize before printing the splash screen on reset. If you are going to
    // do this, then make sure to put delay BEFORE enabling TMR interrupt.
    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while (TMR0_HasOverflowOccured() == false);

    printf("inLab 05\r\n");
    printf("Color Cube\r\n");
    printf("Dev'21 board wiring\r\n");
    printf("RC2 <-> Red LED");
    printf("\r\n> "); // print a nice command prompt

    for (;;) {

        if (EUSART1_DataReady) { // wait for incoming data on USART
            cmd = EUSART1_Read();
            switch (cmd) { // and do what it tells you to do
                case '?':
                    printf("------------------------------\r\n");
                    printf("    Red:   0x%x",CCPR1L.)
                    printf("    Green: 0x%x",CCPR1L.)
                    printf("    Blue:  0x%x",CCPR1L.)
                    printf("------------------------------\r\n");
                    printf("?: Help menu\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("R/r: increase/decrease Red intensity\r\n");
                    printf("G/g: increase/decrease Green intensity\r\n");
                    printf("B/b: increase/decrease Blue intensity\r\n");
                    printf("C/c: start/stop color cycle\r\n");
                    printf("a: All LEDs off");
                    printf("+/-: increase/decrease the color tour speed.\r\n");
                    printf("------------------------------\r\n");
                    break;
                    
                    //--------------------------------------------
                    // Reset the processor after clearing the terminal
                    //--------------------------------------------                      
                case 'Z':
                    for (i = 0; i < 40; i++) printf("\n");
                    RESET();
                    break;

                    //--------------------------------------------
                    // Clear the terminal
                    //--------------------------------------------                      
                case 'z':
                    for (i = 0; i < 40; i++) printf("\n");
                    break;

                case 'R':
                    if (duty <= 0xFF - DUTY_INC) duty += DUTY_INC;
                    EPWM1_LoadDutyValue(duty);
                    printf("The duty cycle is %d TMR2 counts high in a period of %d TMR2 counts.\r\n", duty, 0xFF);
                    break;


                case 'r':
                    if (duty >= DUTY_INC) duty -= DUTY_INC;
                    EPWM1_LoadDutyValue(duty);
                    printf("The duty cycle is %d TMR2 counts high in a period of %d TMR2 counts.\r\n", duty, 0xFF);
                    break;        
                    
                case 'G':
                    
                    break;
                    
                case 'g':
                    
                    break;    
                    
                case 'B':
                    
                    break;

                case 'b':
                    
                    break;

                case 'C':
                    
                    break;

                case 'c':
                    
                    break;

                case 'a':
                    
                    break;

                case '+':
                    
                    break;

                case '-':
                    
                    break;                    
                    //--------------------------------------------
                    // If something unknown is hit, tell user
                    //--------------------------------------------
                default:
                    printf("Unknown key %c\r\n", cmd);
                    break;
            } // end switch
        } // end if
    } // end while 
} // end main                 

void myTMR0ISR(void){
    static tmr0myspace_t tmr0_state = IDLE;
    if(tourColors == true){
        switch(tmr0_state){
            case IDLE: // Idle state, waiting for the playingSong flag to be set to true
                playSong = true; 
                tmr0_state = PLAYING_SONG;
                noteIndex = 0;
                break;
        
            case PLAYING_SONG: // State of playing a note, sets flag to true
                playNote = true; 
                TMR0_WriteTimer(0x10000 - duration[songIndex][noteIndex]);
                tmr0_state = PLAYING_REST;
                break;

            case PLAYING_REST: // State of playing REST between notes, exits the ISR
                TMR0_WriteTimer(0x10000 - REST);
                playNote = false; 
                if (noteIndex == songLength[songIndex]) {  
                    tmr0_state = IDLE;
                    playSong = false;
                }
                else {
                    tmr0_state = PLAYING_SONG;
                    noteIndex++;
                }
                break;
        }
    }
}

void myTMR1ISR(void){
    if(playNote == true){
        if(noteIndex < songLength[songIndex]){
            TMR1_WriteTimer(0x10000 - scale[notes[songIndex][noteIndex]]);
            SPEAKER_PIN_Toggle();        
        } else if(noteIndex == songLength[songIndex]){
            SPEAKER_PIN_SetLow();
        }
    }    
}
