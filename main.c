/*! \file main.c \brief DMX512 Communication. */
//*****************************************************************************
/*
DMX512 Communication

All rights reserved @ Fahad Mirza (fahadmirza80@yahoo.com)

Course Project (Fall 2015)
Embedded System Controller - EE5314
Dr. Jason Losh
Department of Electrical Engineering
University of Texas at Arlington

-----------------------------------------------------------------------------
Notes
-----------------------------------------------------------------------------
File Name	: 'main.c'
Created		: 19th October, 2015
Revised		: 26th November, 2015
Version		: 1.1
Target uC	: TM4C123GH6PM
Clock Source: 16 MHz primary oscillator
Clock Rate	: 40 MHz using PLL
Devices used: UART0 & 1

------------------------------------------------------------------------------------
Objective
------------------------------------------------------------------------------------
CONTROLLER Code:
Get command(string) from PC (RS-232), parse it and send data through RS-485.


-----------------------------------------------------------------------------
 Hardware Target
-----------------------------------------------------------------------------

Hardware configuration:
Blue Backlight LED: PF2 powers the blue LED
Red LED: PF1 drives an NPN transistor that powers the red LED
Green LED: PF3 drives an NPN transistor that powers the green LED
Pushbutton: SW1 pulls pin PF4 low (internal pull-up is used)
UART Interface:
  U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
  The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port
  Configured to 115,200 baud, 8N1
*/



#include "main.h"


//-----------------------------------------------------------------------------
// Interrupt Subroutine
//-----------------------------------------------------------------------------
void Uart1Isr()
{
	if(U1_TX_INT)
	{
		if(txPhase == 0)
		{
			brkFunc();
		}
		else if(txPhase == 1)
		{
			sendStartByte(dataStartByte);
		}
		else
		{
			putcUart1(dmxData[txPhase-1]);
		}
		txPhase = (txPhase+1)%(maxDmxAddr+2);
		U1_TX_INT_FLAG = 1; 						// Clear the flag
	}
	else if(U1_FRAME_INT)
	{
		if(getcUart1() == 0)					// Is it break?
		{
			rxPhase = 1;
			U1_FR_INT_FLAG = 1;
		}
	}
	else if(U1_RX_INT)
	{
		char temp;
		temp = UART1_DR_R & 0xFF;
		if(rxPhase == 1)
		{
			if(temp == 0)				// Is it Start Code == data code?
				rxPhase++;
		}
		else if(rxPhase>1)
		{
			if((rxPhase-1) == deviceAdd)
			{
				if(temp>0)
					BLUE_LED = 1;
				else
					BLUE_LED = 0;

				rxPhase = 0;
			}
			else
				rxPhase++;
		}
	}
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
	// Local variables
	char buffer[6];
	int deviceMode;						//***************** Not sure yet if this needs to be global***********//


	// Initialize hardware
	initHw();
    initUart0();
    initUart1();
    clrDmxData();

    // Step 1
    RED_LED = 1;
    waitMicrosecond(250000);
    RED_LED = 0;
    waitMicrosecond(250000);
    waitPbPress();				// Wait for PB press



	// Display device mode

    deviceMode = readDevMode();

    if(deviceMode == TX_MODE)
    {
    	dmxTxDEN = 1;
    	txPhase = 1;
    	putsUart0("Tx Mode\r\n");
    	enableU1TxINT();
    	brkFunc();
    	while(1)
		{
			getCmd();
		}
    }
    else
    {
    	putsUart0("Rx Mode\r\n");
    	deviceAdd = readDevAdd();
    	ltoa(deviceAdd,buffer);
    	putsUart0("Device Address: ");  putsUart0(buffer);  putsUart0("\r\n");

    	dmxTxDEN = 0;
    	disableU1TxINT();
    	enableU1RxINT();
    	while(1);

    }


}
