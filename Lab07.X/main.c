//--------------------------------------------------------------------
// Name:            Bryce Gelok and James Talbott
// Date:            Fall 2020
// Purp:            Lab 7
//
// 
// Assisted by:     Microchips 18F26K22 Tech Docs 
//-
//- Academic Integrity Statement: I certify that, while others may have
//- assisted me in brain storming, debugging and validating this program,
//- the program itself is my own work. I understand that submitting code
//- which is the work of other individuals is a violation of the course
//- Academic Integrity Policy and may result in a zero credit for the
//- assignment, or course failure and a report to the Academic Dishonesty
//- Board. I also understand that if I knowingly give my original work to
//- another individual that it could also result in a zero credit for the
//- assignment, or course failure and a report to the Academic Dishonesty
//- Board.
//------------------------------------------------------------------------
#include "mcc_generated_files/mcc.h"
#pragma warning disable 520     // warning: (520) function "xyz" is never called  3
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

#define LED_ON      25 
#define LED_OFF     0
#define MAX_BUFFER_SIZE 32

uint8_t userEnter8bit(void);
void decodeIntoASCII(char msg[]);

void myTMR1ISR(void);
char transmitIRBuffer[MAX_BUFFER_SIZE];
uint8_t transmitStart = false;
uint8_t transmitBusy = false;
uint16_t bitPeriodInTMR1Counts = 13333; // 1200 Baud default

void myEUSART2ISR(void);
char recieveIRBuffer[MAX_BUFFER_SIZE];
uint8_t receiveBusy = false;
uint8_t receiveNewMessage = false;
uint16_t baudRate;
uint16_t bitPeriod[5] = {53333, 13333, 6666, 1666, 833};
uint8_t baudRateSelected = 1;
char letter = '0';


//----------------------------------------------
// Main "function"
//----------------------------------------------

void main(void) {

    uint8_t i;
    char cmd;

    SYSTEM_Initialize();
    EPWM2_LoadDutyValue(LED_OFF);

    // BEFORE enabling interrupts, otherwise that while loop becomes an
    // infinite loop.  Doing this to give EUSART1's baud rate generator time
    // to stabelize - this will make the splash screen looks better
    TMR1_WriteTimer(0x0000);
    PIR1bits.TMR1IF = 0;
    while (PIR1bits.TMR1IF == 0);

    // Since EUSART2 is double buffered, clear out any garbage with two reads from those buffers
    if (EUSART2_DataReady) (void) EUSART2_Read();
    if (EUSART2_DataReady) (void) EUSART2_Read();

    TMR1_SetInterruptHandler(myTMR1ISR);
    INTERRUPT_PeripheralInterruptEnable();
    INTERRUPT_GlobalInterruptEnable();


    printf("Lab 7\r\n");
    printf("Receive and decode an IR character\r\n");
    printf("Dev'21 Board wiring\r\n");
    printf("Install a jumper wire from RC0 to RB7 ONLY AFTER unplugging PICKit3\r\n");
    printf("Install a jumper over IR_TX header pins\r\n");
    printf("\r\n> "); // print a nice command prompt

    for (;;) {

        if (EUSART1_DataReady) { // wait for incoming data on USART
            cmd = EUSART1_Read();
            switch (cmd) { // and do what it tells you to do

                    //--------------------------------------------
                    // Reply with help menu
                    //--------------------------------------------
                case '?':
                    printf("-------------------------------------------------\r\n");
                    printf("2400 Baud\r\n");
                    printf("PR2: %d\r\n", PR2);
                    printf("-------------------------------------------------\r\n");
                    printf("?: help menu\r\n");
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("b: set Baud rate\r\n");
                    printf("m: create a NULL terminated message with SRC and DEST prefix.\r\n");
                    printf("s: set source transmit identity\r\n");
                    printf("d: set Destination transmit target\r\n");
                    printf("S: Send message using TMR1 ISR\r\n");
                    printf("R: Receive message using EUSART2 via IR decoder\r\n");
                    printf("M: Monitor all IR traffic.\r\n");
                    printf("x/X: decode tx/RX message.\r\n");
                    printf("-------------------------------------------------\r\n");
                    break;

                    //--------------------------------------------
                    // Reply with "ok", used for PC to PIC test
                    //--------------------------------------------
                case 'o':
                    printf("o:	ok\r\n");
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

                    //--------------------------------------------
                    // Set Baud rate
                    //--------------------------------------------    
                case 'b':
                    printf("Choose the index of the target baud rate\r\n");
                    printf("0: 300 baud\r\n");
                    printf("1: 1200 baud\r\n");
                    printf("2: 2400 baud\r\n");
                    printf("3: 9600 baud\r\n");
                    printf("4: 19200 baud\r\n");
                    while (EUSART1_DataReady);
                    baudRateSelected = EUSART1_Read() - '0';
                    switch (baudRateSelected) {
                        case 0: SPBRGH2 = 0xD0;
                            SPBRG2 = 0x54;
                            break;
                        case 1: SPBRGH2 = 0x34;
                            SPBRG2 = 0x14;
                            break;
                        case 2: SPBRGH2 = 0x1A;
                            SPBRG2 = 0x0A;
                            break;
                        case 3: SPBRGH2 = 0x06;
                            SPBRG2 = 0x82;
                            break;
                        case 4: SPBRGH2 = 0x03;
                            SPBRG2 = 0x40;
                            break;
                        default: SPBRGH2 = 0x34;
                            SPBRG2 = 0x14;
                            break;
                    }
                    printf("Baud rate assigned %02x:%02x\r\n", SPBRGH2, SPBRG2);
                    break;

                    //--------------------------------------------
                    // create a NULL terminated message with SRC and DEST prefix
                    //-------------------------------------------- 
                case 'm':
                {
                    uint8_t ind = 0, cksum = 0;
                    printf("Enter a message, hit return when done.\r\n");
                    printf(">");

                    //Gets user message and creates cksum
                    while (EUSART1_DataReady);
                    for (ind = 2; ind < ((MAX_BUFFER_SIZE - 2)); ind++) {
                        transmitIRBuffer[ind] = EUSART1_Read();
                        cksum += transmitIRBuffer[ind];
                        printf("%c", transmitIRBuffer[ind]);
                        if (transmitIRBuffer[ind] == '\r') {
                            cksum -= '\r';
                            printf("\r\n");
                            break;
                        }
                    }
                    transmitIRBuffer[ind + 1] = '\0';
                    transmitIRBuffer[ind + 2] = cksum;

                    //NOTE: Destination and source addresses are changed in d and s

                    //Output results - Message, cksum, SRC, DES
                    ind = 2;
                    printf("\tMessage: ");
                    for (;;) {
                        if (transmitIRBuffer[ind] == '\0') {
                            printf("\r\n");
                            ind++;
                            break;
                        }
                        printf("%c", transmitIRBuffer[ind]);
                        ind++;
                    }
                    printf("\tchecksum: %d\r\n", transmitIRBuffer[ind]);
                    printf("\tSRC: %d\r\n", transmitIRBuffer[0]);
                    printf("\tDES: %d\r\n", transmitIRBuffer[1]);
                    break;
                }

                    //--------------------------------------------
                    // set Source transmit identity
                    //--------------------------------------------
                case 's':
                    printf("Enter source address: ");
                    transmitIRBuffer[0] = userEnter8bit();
                    printf("SRC: %d\r\n", transmitIRBuffer[0]);
                    break;

                    //--------------------------------------------
                    // set Destination transmit target
                    //--------------------------------------------
                case 'd':
                    printf("Enter destination address: ");
                    transmitIRBuffer[1] = userEnter8bit();
                    printf("DES: %d\r\n", transmitIRBuffer[1]);
                    break;

                    //--------------------------------------------
                    // Send message using TMR1 ISR
                    //--------------------------------------------  
                case 'S':
                	baudRate = bitPeriod[baudRateSelected];
                	uint8_t tindex = 0;
                	for(;;) {
                    	letter = transmitIRBuffer[i];
                    	transmitStart = true;
                        transmitBusy = true;
                        while (transmitBusy);
                    	if (transmitIRBuffer[i] == '\0') {
                        	letter = transmitIRBuffer[i + 1];
                        	transmitCharacterOverIR(letter, baudRate);
                        	break;
                    	}
                    	++i;
                	}
                	printf("Transmitted\r\n");
                    printf("\tChecksum computed:\t%d\r\n", transmitIRBuffer[tindex]);
                    printf("\tSource Identity:\t%d\r\n", transmitIRBuffer[0]);
                    printf("\tDestination:\t%d\r\n", transmitIRBuffer[1]);
                	break;
            
                

                break;    
                    
                    transmitStart = true;
                    transmitBusy = true;
                    while (transmitBusy);
                    printf("Transmitted.\r\n");
                    break;

                    //--------------------------------------------
                    // Receive message using EUSART2
                    //--------------------------------------------
                case 'R':
                    if (receiveNewMessage == true) {
                        printf("Received\r\n");
                        printf("\tMessage: %s\r\n");
                        uint8_t i = 2;
                        while (recieveIRBuffer[i] != '\0') {
                            printf("%c", recieveIRBuffer[i]);
                            i++;
                        }
                        printf("\r\n");
                        ++i;
                        printf("\tChecksum computed:\t%u\r\n", recieveIRBuffer[i]);
                        printf("\tChecksum received:\t%u\r\n", recieveIRBuffer[i]);
                        printf("Source address:\t%u\r\n", recieveIRBuffer[0]);
                        printf("Destination address:\t%u\r\n", recieveIRBuffer[1]);
                        i = 0;
                    } else
                        printf("No message, receiveNewMessage = false\r\n");
                    break;

                    //--------------------------------------------
                    // Decode tx/RX message
                    //--------------------------------------------                
                case 'M':
                    printf("Hit any key to exit.");
                    while (!EUSART1_DataReady);
                    printf("SRC\tDEST\tCHECK\tMESSAGE\r\n");
                    printf("%u\t%u\t%u\t%s\r\n", transmitIRBuffer[0], transmitIRBuffer[1], transmitIRBuffer[2], transmitIRBuffer[1]);
                    break;

                case 'X':
                    printf("RX buffer contents\r\n");
                    decodeIntoASCII(recieveIRBuffer);
                    break;

                case 'x':
                    printf("TX buffer contents\r\n");
                    decodeIntoASCII(transmitIRBuffer);
                    break;

                    //--------------------------------------------
                    // reset EUSART2 in case it needs doing
                    //--------------------------------------------                
                case 'r':
                    RCSTA2bits.CREN = 0; // Try restarting EUSART2
                    RCSTA2bits.CREN = 1;
                    printf("Just reset EUSART2\r\n");
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

typedef enum {
    TX_IDLE, TX_START_BIT, TX_DATA_BITS, TX_STOP_BIT
} myTXstates_t;
//----------------------------------------------
// My TMR1 ISR to handle incoming characters from IR decoder
//----------------------------------------------

void myTMR1ISR(void) {

    static myTXstates_t tmr1ISRstate = TX_IDLE;
    static uint8_t mask = 0b00000001;
    static uint8_t transmitIndex = 0;
    static char letter;
    static bool checkSumSent = false;

    if (transmitStart == true) {

        switch (tmr1ISRstate) {

                //---------------------------------
                // If you are here, it means that you've been inactive
                //---------------------------------
            case TX_IDLE:
                transmitBusy = true;
                checkSumSent = false;
                transmitIndex = 0;
                tmr1ISRstate = TX_START_BIT;
                break;
            case TX_START_BIT:
                EPWM2_LoadDutyValue(LED_ON);
                mask = 0b00000001;
                tmr1ISRstate = TX_DATA_BITS;
                letter = transmitIRBuffer[transmitIndex];
                //---------------------------------
                // Send out 8 data bits
                //---------------------------------
            case TX_DATA_BITS:
                if (mask == 0) {
                    tmr1ISRstate = TX_STOP_BIT;
                    EPWM2_LoadDutyValue(LED_OFF);
                } else {
                    if ((letter & mask) != 0) EPWM2_LoadDutyValue(LED_OFF);
                    else EPWM2_LoadDutyValue(LED_ON);
                }
                mask = mask << 1;
                break;
            case TX_STOP_BIT:
                if (checkSumSent == true) {
                    tmr1ISRstate = TX_IDLE;
                    transmitBusy = false;
                    transmitStart = false;
                    break;
                } else if (letter == '\0') {
                    checkSumSent = true;
                }
                tmr1ISRstate = TX_START_BIT;
                break;
                //---------------------------------
                // How did you get here?
                //---------------------------------
            default:
                tmr1ISRstate = TX_IDLE;
                break;
        }
    }

    TMR1_WriteTimer(0x10000 - bitPeriodInTMR1Counts);
    PIR1bits.TMR1IF = 0;
}

//Gets input for three digit number
//Function doesn't check inputs

uint8_t userEnter8bit() {
    uint8_t num = 0;
    char userNum[3] = {0, 0, 0};
    uint8_t i;
    for (i = 0; i < 3; ++i) {
        while (!EUSART1_DataReady);
        char cmd = EUSART1_Read();
        if (cmd == '\r') break;
        printf("%c", cmd);
        userNum[i] = cmd;
    }
    switch (i) {
        case 1:
            num = userNum[0] - 48;
            break;
        case 2:
            num = (10 * (userNum[0] - 48)) + (userNum[1] - 48);
            break;
        default:
            num = (100 * (userNum[0] - 48)) + (10 * (userNum[1] - 48)) + (userNum[2] - 48);
            break;
    }
    printf("\r\n");
    return num;
}

void decodeIntoASCII(char msg[]) {
    uint8_t i = 0;
    do {
        ++i;
        printf("\t%d:", i);
        printf("\t%h", msg[i]);
        printf("\t%c", msg[i]);
        printf("\r\n");
    } while (msg[i - 1] != '\0');
}