//*****************************************************************
// Name:Bryce Gelok and James Talbott
// Date:	Fall 2020
// Lab: 	06
// Purp:	Receive and decode an IR packet
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
#pragma warning disable 520 	// warning: (520) function "xyz" is never called  3
#pragma warning disable 1498	// fputc.c:16:: warning: (1498) pointer (unknown)

//best line
#define LED_ON  	25
#define LED_OFF 	0
#define MAX_BUFFER_SIZE 25

void myEUSART2ISR(void);
char IRreceiveBuffer[MAX_BUFFER_SIZE];
char IRtransmitBuffer[MAX_BUFFER_SIZE];

void transmitCharacterOverIR(char letter, uint16_t baudRate);

uint8_t receiveBusy = false;
uint8_t receiveNewMessage = false;
uint8_t baudRateSelected = 1;
uint16_t bitPeriod[5] = {53333, 13333, 6666, 1666, 833};
uint8_t ind = 0;
uint8_t cksum;
uint8_t checksum;



//----------------------------------------------
// Main "function"
//----------------------------------------------

void main(void) {

	uint8_t i;
	char cmd;
	char letter;

	SYSTEM_Initialize();
	EUSART2_SetRxInterruptHandler(myEUSART2ISR);
	INTERRUPT_GlobalInterruptEnable();
	INTERRUPT_PeripheralInterruptEnable();

	EPWM2_LoadDutyValue(LED_OFF);

	// Since EUSART2 is double buffered, clear out any garbage with two reads from those buffers
	if (EUSART2_DataReady) (void) EUSART2_Read();
	if (EUSART2_DataReady) (void) EUSART2_Read();

	// Delay so the Baud rate generator is stable and prints the splash screen correctly
	TMR1_WriteTimer(0);
	PIR1bits.TMR1IF = 0;
	while (TMR1_HasOverflowOccured() == false);

	//PIE3bits.RC2IE = 0;
	//EUSART2_SetRxInterruptHandler(myEUSART2ISR);
	//PIE3bits.RC2IE = 1;    
	//INTERRUPT_PeripheralInterruptEnable();
	//INTERRUPT_GlobalInterruptEnable();

	printf("Lab 6\r\n");
	printf("Receive and decode an IR packet\r\n");
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
            	{
                	printf("-------------------------------------------------\r\n");
                	switch (baudRateSelected) {
                    	case 0: printf("300 Baud\r\n");
                        	break;
                    	case 1: printf("1200 Baud\r\n");
                        	break;
                    	case 2: printf("2400 Baud\r\n");
                        	break;
                    	case 3: printf("9600 Baud\r\n");
                        	break;
                    	case 4: printf("19200 Baud\r\n");
                        	break;
                    	default: printf("1200 Baud\r\n");
                        	break;
                	}
                	printf("-------------------------------------------------\r\n");
                	printf("?: help menu\r\n");
                	printf("o: k\r\n");
                	printf("Z: Reset processor\r\n");
                	printf("z: Clear the terminal\r\n");
                	printf("b: set the Baud rate of the sent characters\r\n");
                	printf("r: reset EUSART2\r\n");
                	printf("m: Enter in the transmit Message\r\n");
                	printf("S: Send message in foreground\r\n");
                	printf("R: Receive message using EUSART2 ISR via IR decoder\r\n");
                	printf("-------------------------------------------------\r\n");
                	break;
            	}
                	//--------------------------------------------
                	// Reply with "ok", used for PC to PIC test
                	//--------------------------------------------
            	case 'o':
            	{
                	printf("o:	ok\r\n");
                	break;
            	}
                	//--------------------------------------------
                	// Reset the processor after clearing the terminal
                	//--------------------------------------------                 	 
            	case 'Z':
            	{
                	for (i = 0; i < 40; i++) printf("\n");
                	RESET();
                	break;
            	}
                	//--------------------------------------------
                	// Clear the terminal
                	//--------------------------------------------                 	 
            	case 'z':
            	{
                	for (i = 0; i < 40; i++) printf("\n");
                	break;
            	}
                	//--------------------------------------------
                	// Set the Baud rate - use MCC EUSART2 configuration register tab       	 
                	//--------------------------------------------               	 
            	case 'b':
            	{
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
            	}
                	//--------------------------------------------
                	// reset EUSART2 in case it needs doing
                	//--------------------------------------------           	 
            	case 'r':
            	{
                	RCSTA2bits.CREN = 0; // Try restarting EUSART2
                	RCSTA2bits.CREN = 1;
                	printf("Just reset EUSART2\r\n");
                	break;
            	}
                	//--------------------------------------------
                	// Enter in transmit message
                	//--------------------------------------------  
            	case 'm':
            	{
                	ind = 0;
                	cksum = 0;
                	printf("Enter a message, hit return when done.\r\n");
                	printf(">");
                	while (EUSART1_DataReady);
                	for (ind = 0; ind < ((MAX_BUFFER_SIZE - 2)); ind++) {
                    	IRtransmitBuffer[ind] = EUSART1_Read();
                    	cksum += IRtransmitBuffer[ind];
                    	printf("%c", IRtransmitBuffer[ind]);
                    	if (IRtransmitBuffer[ind] == '\r') {
                        	cksum -= '\r';
                        	printf("\r\n");
                        	break;
                    	}
                	}
                	IRtransmitBuffer[ind + 1] = '\0';
                	IRtransmitBuffer[ind + 2] = cksum;
                	printf("Message: \r\n\t%s\r\n", IRtransmitBuffer);
                	printf("\tchecksum: %d\r\n", cksum);
                	break;
            	}

            	case 'R':
            	{
                	checksum = 0;
                	if (receiveNewMessage == true) {
                    	printf("Received\r\n");
                    	printf("\tMessage: %s\r\n", IRreceiveBuffer);
                    	i = 0;
                    	while (IRreceiveBuffer[i] != '\0') {
                        	i++;
                    	}
                    	++i;
                    	checksum = IRreceiveBuffer[i];
                    	printf("\tChecksum: %u\r\n", checksum);
                    	receiveNewMessage = false;
                	} else
                    	printf("No message, receiveNewmessage = false\r\n");
                	break;
            	} //--------------------------------------------
                	//Send message in foreground
                	//--------------------------------------------  
            	case 'S':
            	{
                	uint16_t baud_rate = bitPeriod[baudRateSelected];
                	uint8_t i = 0;
                	for(;;) {
                    	letter = IRtransmitBuffer[i];
                    	transmitCharacterOverIR(letter, baud_rate);
                    	if (IRtransmitBuffer[i] == '\0') {
                        	letter = IRtransmitBuffer[i + 1];
                        	transmitCharacterOverIR(letter, baud_rate);
                        	break;
                    	}
                    	++i;
                	}
                	printf("Transmitted\r\n");
                	printf("\tMessage: %s\r\n", IRtransmitBuffer);
                	printf("\tChecksum: %u\r\n", cksum);
                	break;
            	}
                	//--------------------------------------------
                	// If something unknown is hit, tell user
                	//--------------------------------------------
            	default:
                	printf("Unknown key %c\r\n", cmd);
                	break;

        	} // end switch
    	} // end if
	} // end for
}// end main

typedef enum {
	IDLE, DATA, CHECKSUM
} eusart2_t;

void myEUSART2ISR(void) {
	static eusart2_t state = IDLE;
	static uint8_t Index;
	switch (state) {
    	case IDLE: //Idle state
        	state = DATA;
        	Index = 0;
        	IRreceiveBuffer[Index++] = RCREG2;
        	break;

    	case DATA:

        	IRreceiveBuffer[Index++] = RCREG2;
        	if (IRreceiveBuffer[Index - 1] == '\0') {
            	state = CHECKSUM;
        	}
        	break;

    	case CHECKSUM:

        	IRreceiveBuffer[Index++] = RCREG2;
        	receiveBusy = false;
        	receiveNewMessage = true;
        	state = IDLE;
        	break;
	}
}

void transmitCharacterOverIR(char letter, uint16_t baudRate) {
	uint8_t mask = 0b00000001;
	EPWM2_LoadDutyValue(LED_ON);
	TMR1_WriteTimer(0x10000 - baudRate);
	PIR1bits.TMR1IF = 0;
	while (TMR1_HasOverflowOccured() == false);

	while (mask != 0) {
    	if ((letter & mask) != 0) EPWM2_LoadDutyValue(LED_OFF);
    	else EPWM2_LoadDutyValue(LED_ON);
    	mask = mask << 1;
    	TMR1_WriteTimer(0x10000 - baudRate);
    	PIR1bits.TMR1IF = 0;
    	while (TMR1_HasOverflowOccured() == false);
	}

	EPWM2_LoadDutyValue(LED_OFF);
	TMR1_WriteTimer(0x10000 - baudRate);
	PIR1bits.TMR1IF = 0;
	while (TMR1_HasOverflowOccured() == false);
}





