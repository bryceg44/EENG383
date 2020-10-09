//*****************************************************************
// Name:Bryce Gelok and James Talbott
// Date:	Fall 2020
// Lab: 	04
// Purp:	Use a terminal interface to play O Canada
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

#define NUM_SONGS 4
#define MAX_LENGTH 21
#define NUM_OCTAVES 	2


#define QUA 	15625   // 0.25 seconds
#define HAL 	(QUA * 2)  
#define WHO 	(QUA * 4)  
#define EIG 	(QUA / 2)  
#define SIX 	(QUA / 4)
#define THIR	(QUA / 8)
#define DOTTEDQUA  (QUA + EIG)
#define DOTTEDHAL  (HAL + QUA)
#define DELTA   (HAL)

#define A4  	0
#define AS4 	1
#define BF4 	1
#define B4  	2
#define C5  	3
#define CS5 	4
#define DF5 	4
#define D5  	5
#define DS5 	6
#define EF5 	6
#define E5  	7
#define F5  	8
#define FS5 	9
#define GF5 	9
#define G5  	10
#define GS5 	11
#define AF5 	11
#define A5  	12
#define AS5 	13
#define BF5 	13
#define B5  	14
#define C6  	15
#define CS6 	16
#define DF6 	16
#define D6  	17
#define DS6 	18
#define EF6 	18
#define E6  	19
#define F6  	20
#define FS6 	21
#define GF6 	21
#define G6  	22
#define GS6 	23
#define AF6 	23
#define A6  	24

#define REST	25

uint8_t playSong = false;
uint8_t playNote = false;
static uint8_t myState_t;
uint8_t noteIndex;
uint8_t songIndex;

// two octaves worth of notes
uint16_t scale[NUM_OCTAVES*12] =  {18182,17167,16194,15296,14440,13629,12862,12140,
                              	11461,10811,10204,9627,9091,8584,8105,7648,7220,
                              	6814,6431,6070,5731,5409,5105,4819,4548};

uint8_t songLength[NUM_SONGS] = {21, 3, 8, 8};

uint16_t notes[NUM_SONGS][MAX_LENGTH] = {
    {A5, C6, C6, F5, G5, A5, BF5, C6, D6, G5, A5, A6, B5, B5, C6, D6, E6, E6, D6, D6, C6},
    {D5, F5, D6},
    {E6, F6, E6, F6, E6, C6, A4, A4},
    {E6, F6, E6, F6, E6, C6, A5, A6}
};

uint16_t duration[NUM_SONGS][MAX_LENGTH] = {
    {HAL, DOTTEDQUA, EIG, DOTTEDHAL, QUA, QUA, QUA, QUA, QUA, DOTTEDHAL, QUA, HAL, DOTTEDQUA, EIG, DOTTEDHAL, QUA, QUA, QUA,QUA,QUA, DOTTEDHAL},
    {QUA, QUA, QUA},
    {QUA, QUA, QUA, QUA, QUA, QUA, QUA},
    {HAL, HAL, HAL, HAL, HAL, HAL, HAL}

}; // 1:1 timer counts


void myTMR1ISR(void);
void myTMR0ISR(void);

void main(void) {

	uint8_t i;
	char cmd;
    songIndex = 0;
    noteIndex = 0;
	SYSTEM_Initialize();
    
	INTERRUPT_GlobalInterruptEnable();
	INTERRUPT_PeripheralInterruptEnable();

	TMR1_SetInterruptHandler(myTMR1ISR);
	TMR0_SetInterruptHandler(myTMR0ISR);
    
	printf("inLab 04\r\n");
	printf("Interrupt Music Box\r\n");
	printf("Dev'21\r\n");
	printf("Board wiring\r\n");
	printf("RB5 -> LPFin and install LPFout/AMPin jumper");
	printf("\r\n> "); // print a nice command prompt

	for (;;) {

    	if (EUSART1_DataReady) { // wait for incoming data on USART
        	cmd = EUSART1_Read();
        	switch (cmd) { // and do what it tells you to do
            	case '?':
                	printf("------------------------------\r\n");
                	printf("?: Help menu\r\n");
                	printf("o: k\r\n");
                	printf("Z: Reset processor\r\n");
                	printf("z: Clear the terminal\r\n");
                	printf("p: Play song once\r\n");
                	printf("r: Rhythm practice\r\n");
                	printf("------------------------------\r\n");
                	break;
                	//--------------------------------------------
                	// Start playing song
                	//--------------------------------------------                                     	 
            	case 'p':
                	printf("Start playing song.\r\n");
                    songIndex = 0;
                	playSong = true;
                	break;

               	 
                	//--------------------------------------------
                	// Rhythm Practice
                	//--------------------------------------------                                                         	 
                    
                	//--------------------------------------------
                	// Simple handshake with the development board
                	//--------------------------------------------                                     	 
            	case 'o':
                	printf("k.\r\n");
                	break;

                	//--------------------------------------------
                	// Reset the processor after clearing the terminal
                	//--------------------------------------------  
            	case 'r':
                	printf("Listen to the beat pattern.\r\n");
                	songIndex = 1;
                    playSong = true;
                	while(playSong == true);
                	printf("Use the upper button to reproduce this pattern\r\n");
                	INTCONbits.TMR0IE = 0;
                	uint8_t success = true;
                	uint16_t initial, final, sum = 0;
                	for(noteIndex = 0; noteIndex < songLength[songIndex]; ++noteIndex) {
                    	while(BUTTON_PIN_GetValue() == 1);
                        initial = TMR0_ReadTimer();
                        playNote = true;
                        
                    	while(BUTTON_PIN_GetValue() == 0);
                    	final = TMR0_ReadTimer();
                    	playNote = false;
                        
                    	sum = final - initial;
                    	if ( (sum < (duration[noteIndex] + THIR - DELTA)) || (sum < (duration[noteIndex] + THIR - DELTA))) {
                        	success = false;
                    	}
                	}
                    switch(success){
                        case(false):
                            printf("You Lose!\r\n");
                            songIndex = 2;
                            playSong = true;
                            break;
                            
                        case(true):
                            printf("You Win!\r\n");
                            songIndex = 3;
                            playSong = true;
                            break;
                    }
                    
                	INTCONbits.TMR0IE = 1;
                    noteIndex = 0;
                	break;

                	//--------------------------------------------
                	// Reset the processor after clearing the terminal
                	//--------------------------------------------                  	 
            	case 'Z':
                	for (i = 0; i < 40; i++) printf("\r\n");
                	RESET();
                	break;

                	//--------------------------------------------
                	// Clear the terminal
                	//--------------------------------------------                 	 
            	case 'z':
                	for (i = 0; i < 40; i++) printf("\r\n");
                	break;

                	//--------------------------------------------
                	// If something unknown is hit, tell user
                	//--------------------------------------------
            	default:
                	printf("Unknown key %c\r\n", cmd);
                	break;
        	} // end switch
    	} // end if   	 
	} // end infinite loop    
} // end main

typedef enum {IDLE, PLAYING_SONG, PLAYING_REST} tmr0myspace_t;

void myTMR0ISR(void){
    static tmr0myspace_t tmr0_state = IDLE;
    if(playSong == true){
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