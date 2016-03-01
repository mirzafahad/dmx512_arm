/* 
DMX512 Communication Protocol

Spring 2014

All rights @ 
Fahad Mirza (fahad.mirza34@mavs.uta.edu)

Course Project
Embedded System Controller - EE5314
Dr. Jason Losh
Department of Electrical Engineering
University of Texas at Arlington

DEVICE code

-----------------------------------------------------------------------------
Notes             
-----------------------------------------------------------------------------
File Name	: 'main.c'
Created		: 4th March, 2014
Revised		: 28th April, 2014
Version		: 1.0
Target uC	: 33FJ128MC802
Clock Source: 8 MHz primary oscillator set in configuration bits
Clock Rate	: 80 MHz using prediv=2, plldiv=40, postdiv=2
Devices used: UART

-----------------------------------------------------------------------------
Objective             
-----------------------------------------------------------------------------
Get instruction(RS485) from CONTROLLER, and execute.

-----------------------------------------------------------------------------
Hardware description          
-----------------------------------------------------------------------------
Green LED: Anode connected through 100ohm resistor to RB4 (pin 11), cathode 
grounded.

Red LED: anode connected through 100ohm resistor to RB5 (pin 14), cathode 
grounded.

Device Address: RB3,RA4,RB9-RB15 are connected with 9 pin Dip switch. 

*/


#include <p33FJ128MC802.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "uart1.h"
#include "uart2.h"


#define dmxWrOn LATBbits.LATB8			 	// RS485 Read/Write Enable Pin RB8 (pin17)
#define BAUD_19200 129                       // brg for low-speed, 40 MHz clock // UART1-->round((40000000/16/19200)-1)
#define BAUD_250K 9							// UART2-->(40M/16/250K)-1
#define BAUD_96153 25						// UART2-->(40M/16/96513)-1 
											// For Break & MAB. This baudrate will give 20us MAB and 92us Break 


//-------- Delay functions --------//
extern void wait_us(unsigned int n);
extern void wait_ms(unsigned int n);


// Global Variables
unsigned int devAdd;			// Device Address Variable


// Timer1 Interrupt Variables
volatile int redTimeout,grnTimeout,noDataTimeout;		// LED blink and Invalid DMX timeout variables.


// UART1 Tx interrupt variables
unsigned char txBuf[256];			// Ring Buffer
unsigned char wr_index=0;			// Tx Buffer write & read index
unsigned char rd_index=0;			// unsigned char so after 255 they automatically return to zero.
int FULL=0;							// Buffer full flag


//-----------------------------------------------------------------------------
// Interrupt Subroutines                
//-----------------------------------------------------------------------------

// For UART1 Tx
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
	if(rd_index != wr_index)			// Is there any data available to send?
	{
		U1TXREG = txBuf[rd_index++];	// Put data into Tx buffer and increse index by 1
		FULL = 0;						// if the buffer was FULL, now at least one space available to store new byte in the Txbuff.
	}

	IFS0bits.U1TXIF = 0;				// Clear the flag
}



// For TIMER1
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void)
{
	TMR1 = 0;
	if(redTimeout>0)
	{
		redTimeout--;
		if(redTimeout <= 0) LATBbits.LATB5 = 0;
	}

	if(grnTimeout>0)
	{
		grnTimeout--;
		if(grnTimeout <= 0) LATAbits.LATA0 = 1;
	}

	if(noDataTimeout>0)
	{
		noDataTimeout--;
		if(noDataTimeout <= 0) LATAbits.LATA0 = 0;
	}
	// clear IF
	IFS0bits.T1IF = 0;
 }


//-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-
// Subroutines                
//-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-


//*********************//
// Initialize Hardware //
//*********************//
void init_hw()
{
  PLLFBDbits.PLLDIV = 38;                   // PLL feedback divider = 40;
  CLKDIVbits.PLLPRE = 0;                    // PLL pre divider = 2
  CLKDIVbits.PLLPOST = 0;                   // PLL post divider = 2

  AD1PCFGL = 0x003F;						// Disable all ANalog.(RB2,RB3 & RA0,RA1)Keep RB2 n 3 as o/p (zero tris). They are connected with LEDs					
  TRISB |= 0xFE00;							// RB9-RB15 --> i/p
  TRISAbits.TRISA4 = 1;						// RA4 --> i/p
  TRISAbits.TRISA1 = 1;						// RA1 --> i/p
  
  CNPU1 |= 0xF809;							// Enable pull-up for DIP Switch 
  CNPU2bits.CN16PUE = 1;
  CNPU2bits.CN21PUE = 1;		 

  LATAbits.LATA0 = 0;                       // Write 0 into RA0 output latche
  LATBbits.LATB5 = 0;						// Write 0 into RB5 output latche
  TRISAbits.TRISA0 = 0;                     // Make green led pin an output
  TRISBbits.TRISB5 = 0;                     // Make red led pin an output

  LATBbits.LATB8 = 0;						 // Write 0 into RB8 output latche
  TRISBbits.TRISB8 = 0;                      // Make DEn pin an output
  RPINR19bits.U2RXR = 7;                     // Assign U2RX to RP7
  RPOR3bits.RP6R = 5;                        // Assign U2TX to RP6

  RPOR1bits.RP2R = 18; //0x12                // connect OC1 to RP2 (PWM)
  RPOR1bits.RP3R = 19; //0x12                // connect OC2 to RP3 (PWM)
  RPOR2bits.RP4R = 20; //0x12                // connect OC3 to RP4 (PWM)

  RPINR18bits.U1RXR = 1;                     // assign U1RX to RP1
  RPOR0bits.RP0R = 3;                        // assign U1TX to RP0
}


void pwm_init()
{
  // Clock timer 2 with internal 40 MHz clock
  // and set period to 1.63ms
  T2CONbits.TCS = 0;	// Select Fcy
  T2CONbits.TCKPS = 3;	// Prescaler 256
  T2CONbits.TON = 1;
  PR2 = 255;
  
  OC1CON = 0;			// turn-off OC1 to make sure changes can be applied
  OC1R = 0;				// set first cycle d.c.
  OC1RS = 0;			// set ongoing d.c. to 0
  OC1CONbits.OCTSEL = 0;// Select Timer 2 as source
  OC1CONbits.OCM = 6;	// Set OC1 to PWM mode

  OC2CON = 0;			// turn-off OC2 to make sure changes can be applied
  OC2R = 0;				// set first cycle d.c.
  OC2RS = 0;			// set ongoing d.c. to 0
  OC2CONbits.OCTSEL = 0;// Select Timer 2 as source
  OC2CONbits.OCM = 6;	// Set OC2 to PWM mode

  OC3CON = 0;			// turn-off OC3 to make sure changes can be applied
  OC3R = 0;				// set first cycle d.c.
  OC3RS = 0;			// set ongoing d.c. to 0
  OC3CONbits.OCTSEL = 0;// Select Timer 2 as source
  OC3CONbits.OCM = 6;	// Set OC3 to PWM mode
}


void pwm1_setdc(unsigned int dc)
{
  OC1RS = dc;
}

void pwm2_setdc(unsigned int dc)
{
  OC2RS = dc;
}

void pwm3_setdc(unsigned int dc)
{
  OC3RS = dc;
}

//*******************//
// Initialize Timer1 //
//*******************//
void timer1_init(unsigned int period)
{
  TMR1 = 0;				// Clear counter
  // Clock timer 1 with internal 40/64 MHz clock 
  T1CONbits.TCS = 0;	// Select Fcy
  T1CONbits.TCKPS = 2;	// Prescaler 64
  T1CONbits.TON = 1;
  PR1 = period;			
  
  IFS0bits.T1IF = 0;	// Clear Flag
  IEC0bits.T1IE = 1;	// Enable timer 1 interrupts
}


//********************************************//
// Put data in 'txBuf' for UART1 Tx interrupt //
//********************************************//
void send_string(const char str[])
{
	int i=0;
	
	while (str[i] != 0 && !FULL)			// Write to buffer if it isn't FULL
	{
		txBuf[wr_index++] = str[i++];
		if((wr_index+1)%256 == rd_index)	// Is next byte going to overwrite the buffer?
			FULL = 1;						// Then SET FULL
		else 
			FULL = 0;
	}
	if(U1STAbits.UTXBF == 0) 				// Is TX FIFO is completely empty?
		U1TXREG = txBuf[rd_index++];		// Then at least write one byte to FIFO to start interrupt
}


//****************************************************//
// Read Device Address 								  //
//----------------------------------------------------//
// 9bit address. From RB15 to RB9,then RA4, then RB3. //
//****************************************************//
void readDevAdd()
{
   devAdd = (PORTB & 0xFE00)>>7;			// Read frm RB15 to RB9
   devAdd |= (PORTAbits.RA4)<<1;			// Read RA4
   devAdd |= PORTAbits.RA1;					// Read RA1
   devAdd = (~devAdd)&0x01FF;				// Active Low. So take compliment.
   devAdd += 1;
}


//*******************************//
// DMX Break generation function //
//*******************************//
void brkFunc()
{
	// Initiating BREAK and MAB
	while(!U2STAbits.TRMT);			// Wait till Tx buffer empty		
	dmxWrOn = 1;					// DMX Write Enable
	U2BRG = BAUD_96153;				// Slow the Baud Rate 
	uart2_putc(0);					// Sending Break and MAB
	while(!U2STAbits.TRMT);			// Wait till Brk & MAB transmitted
	// Break and MAB end
			
	U2BRG = BAUD_250K;				// Return to default speed.
}


//**********************************//
// Retrive the poll data from RS485 //
//**********************************//
int retriveData()
{
	unsigned int index = 1;		// Index Variable for DMX buffer
   	unsigned char data,temp;	

	while(index<513)			// Read all 512 data
	{
		temp = uart2_getc();
		if(index == devAdd)		// Is the data is for this device?
		{
			data = temp;	
		}
		index++;
	}

	return data;
}



//----------------------------------------------------------------------------
// MAIN starts here
//----------------------------------------------------------------------------

int main()
{ 
   // Variable Declarations
   unsigned char temp;
   unsigned char dmxData[3]={0,0,0};	// DMX Data
   unsigned char pollData;			// Poll data
   unsigned int dmxRdIndex;			// Keep track of dmxData
   int brkFlag = 0;						// Indication of Break



   // Initializations...
   init_hw();                 		// Initialize hardware
   pwm_init();	
   uart1_init(BAUD_19200);			// Configure uart1
   uart2_init(BAUD_250K);			// Configure uart2
   timer1_init(625);				// (40M/64)*1m = 625
   
   dmxWrOn = 0; 					// DMX Read On

   LATAbits.LATA0 = 1;         		// Blink green LED for 500ms, Step 1
   wait_ms(500);
   LATAbits.LATA0 = 0;

   //send_string("Welcome");

   while(1)
   {
		readDevAdd();							// Read Device Address
		dmxWrOn = 0;
		if(U2STAbits.URXDA)						// Is there any data in USART2
		{
			if(U2STAbits.FERR)					// Is it with frame error?
			{	
				if(uart2_getc() == 0)			// Is it Break?
				{
					temp = uart2_getc();
					if(temp == 0)				// Is start code is zero?
					{
						brkFlag = 1;			// Got the Break, set the flag.
						dmxRdIndex = 1;			// Initialize the Index
						noDataTimeout = 1000;	// If next break isn't within 1s, then turn off the Grn LED in Timer.
						if(grnTimeout <= 0)		// Solid LED, coz we have valid dmx data (set LED only when grnTimeout is zero)
							LATAbits.LATA0 = 1;
					}
					else if(temp == 0xF0)		// Is it POLL code?
					{
						pollData = retriveData();
						if(pollData)
						{
							dmxWrOn = 1; 				// DMX Write On
							
							wait_us(1);					// Give time to properly convert from read to write mode

							brkFunc();					// Send Break
							redTimeout = 250;			// Set RED LED, indicate break is sent
							LATBbits.LATB5 = 1;
							dmxWrOn = 0; 				// DMX Read On
						}
						else
						{
							dmxWrOn = 0; 				// Make sure Read mode is ON
						}
					}
				}
			}
			else if(brkFlag)					// Do we have a valid Break?
			{	
				temp = uart2_getc();			// Read the data
				if(dmxRdIndex == devAdd)		// Is the data for the device?
				{	
					if(temp != dmxData[0])
					{
						grnTimeout = 250;
						LATAbits.LATA0 = 0;
						dmxData[0] = temp;			// Save the data
						pwm3_setdc(temp);
					}
					temp = uart2_getc();
					if(temp != dmxData[1])
					{
						grnTimeout = 250;
						LATAbits.LATA0 = 0;
						dmxData[1] = temp;			// Save the data
						pwm1_setdc(temp);
					}

					temp = uart2_getc();
					if(temp != dmxData[2])
					{
						grnTimeout = 250;
						LATAbits.LATA0 = 0;
						dmxData[2] = temp;			// Save the data
						pwm2_setdc(temp);
					}
					
					brkFlag = 0;				// Reset the Break flag i.e. again wait for break
					dmxRdIndex = 0;				// Reset the DMX data index. Not Necessary!
				}

				if (++dmxRdIndex>=513) 			// Avoid overflow of the index
					dmxRdIndex = 0;
			}
			else
			{	
				uart2_getc();					// To avoid overrun keep reading the data.
			}
		}
   }
   
   return 0;
}
