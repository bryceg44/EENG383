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


#define DUTY_INC	0x0f
#define NUM_COLOR   3
#define MAX 0xFF9C
#define MIN 0x64

uint8_t colorTour = false;
uint16_t timerInterval = 0x3E8;

#if (NUM_COLOR == 6)
uint16_t initRed[NUM_COLOR] = {0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF};
uint16_t initGre[NUM_COLOR] = {0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00};
uint16_t initBlu[NUM_COLOR] = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF};

uint16_t deltaRed[NUM_COLOR] = {0x0000, 0x0000, 0x0001, 0x0000, 0x0000, 0xffff};
uint16_t deltaGre[NUM_COLOR] = {0x0001, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000};
uint16_t deltaBlu[NUM_COLOR] = {0x0000, 0xFFFF, 0x0000, 0x0000, 0x0001, 0x0000};
#else
uint16_t initRed[NUM_COLOR] = {0x00, 0xFF, 0x00};
uint16_t initGre[NUM_COLOR] = {0x00, 0x00, 0xFF};
uint16_t initBlu[NUM_COLOR] = {0xFF, 0x00, 0x00};

uint16_t deltaRed[NUM_COLOR] = {0x0001, 0xFFFF, 0x0000};
uint16_t deltaGre[NUM_COLOR] = {0x0000, 0x0001, 0xFFFF};
uint16_t deltaBlu[NUM_COLOR] = {0xFFFF, 0x0000, 0x0001};
#endif



void myTMR0ISR(void);

void main(void) {

    char cmd;

    uint16_t dutyRed = 256;
    uint16_t dutyGre = 256;
    uint16_t dutyBlu = 256;

    SYSTEM_Initialize();
    TMR0_SetInterruptHandler(myTMR0ISR);
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    CCPR1L = CCPR2L = CCPR3L = 0xFF;

    EPWM1_LoadDutyValue(dutyRed);
    EPWM2_LoadDutyValue(dutyGre);
    EPWM3_LoadDutyValue(dutyBlu);

    printf("Lab 05\r\n");
    printf("Color Cube\r\n");
    printf("Connect jumper wires\r\n");
    printf("RC2 <-> Red LED\r\n");
    printf("RC1 <-> Green LED\r\n");
    printf("RB5 <-> Blue LED\r\n");
    printf("\r\n"); // print a nice command prompt

    for (;;) {
        if (EUSART1_DataReady) { // wait for incoming data on USART
            cmd = EUSART1_Read();
            switch (cmd) { // and do what it tells you to do
                case '?':
                    printf("------------------------------\r\n");
                    printf("	Red:   0x%x\r\n", CCPR1L);
                    printf("	Green: 0x%x\r\n", CCPR2L);
                    printf("	Blue:  0x%x\r\n", CCPR3L);
                    printf("------------------------------\r\n");
                    printf("?: Help menu\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("R/r: increase/decrease Red intensity\r\n");
                    printf("G/g: increase/decrease Green intensity\r\n");
                    printf("B/b: increase/decrease Blue intensity\r\n");
                    printf("C/c: start/stop color cycle\r\n");
                    printf("a: All LEDs off\r\n");
                    printf("+/-: increase/decrease the color tour speed.\r\n");
                    printf("------------------------------\r\n");
                    break;

                    //--------------------------------------------
                    // Reset the processor after clearing the terminal
                    //--------------------------------------------                 	 
                case 'Z':
                    for (uint8_t i = 0; i < 40; i++) printf("\n");
                    RESET();
                    break;

                    //--------------------------------------------
                    // Clear the terminal
                    //--------------------------------------------                 	 
                case 'z':
                    for (uint8_t i = 0; i < 40; i++) printf("\n");
                    break;

                case 'r':
                    if (dutyRed <= 0xFF - DUTY_INC) dutyRed += DUTY_INC;
                    EPWM1_LoadDutyValue(dutyRed);
                    break;

                case 'R':
                    if (dutyRed >= DUTY_INC) dutyRed -= DUTY_INC;
                    EPWM1_LoadDutyValue(dutyRed);
                    break;

                case 'g':
                    if (dutyGre <= 0xFF - DUTY_INC) dutyGre += DUTY_INC;
                    EPWM2_LoadDutyValue(dutyGre);
                    break;

                case 'G':
                    if (dutyGre >= DUTY_INC) dutyGre -= DUTY_INC;
                    EPWM2_LoadDutyValue(dutyGre);
                    break;

                case 'b':
                    if (dutyBlu <= 0xFF - DUTY_INC) dutyBlu += DUTY_INC;
                    EPWM3_LoadDutyValue(dutyBlu);
                    break;

                case 'B':
                    if (dutyBlu >= DUTY_INC) dutyBlu -= DUTY_INC;
                    EPWM3_LoadDutyValue(dutyBlu);
                    break;

                case 'C':
                    printf("Start\r\n");
                    if (!colorTour) {
                        dutyRed = initRed[0];
                        dutyGre = initGre[0];
                        dutyBlu = initBlu[0];
                    }
                    timerInterval = 0x3E8;
                    colorTour = true;
                    break;

                case 'c':
                    colorTour = false;
                    break;

                case 'a':
                    colorTour = false;
                    EPWM1_LoadDutyValue(256);
                    EPWM2_LoadDutyValue(256);
                    EPWM3_LoadDutyValue(256);
                    break;

                case '+':
                    if (timerInterval <= MIN) {
                        printf("timeInterval is at its max\r\n");
                    } else {
                        timerInterval -= 0x64;
                    }
                    break;

                case '-':
                    if (timerInterval >= MAX) {
                        printf("timeInterval is at its min\r\n");
                    } else {
                        timerInterval += 0x64;
                    }
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

void myTMR0ISR(void) {
    static uint8_t step = 0;
    static uint8_t edge = 0;
    static uint16_t redVal = 0x00; //starts at yellow
    static uint16_t greVal = 0x00;
    static uint16_t bluVal = 0xff;

    if (colorTour == true) {
        TMR0_WriteTimer(0x10000 - timerInterval);
        redVal = redVal + deltaRed[edge];
        greVal = greVal + deltaGre[edge];
        bluVal = bluVal + deltaBlu[edge];

        EPWM1_LoadDutyValue(redVal);
        EPWM2_LoadDutyValue(greVal);
        EPWM3_LoadDutyValue(bluVal);

        step = step + 1;

        if (step == 255) {
            step = 0;
            edge = edge + 1;
            if (edge == NUM_COLOR) {
                edge = 0;
            }
        }
    }
}