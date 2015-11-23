/*! \file main.c \brief DMX512 Communication. */
//*****************************************************************************
/*
DMX512 Communication

All rights reserved @ Fahad Mirza (fahad.mirza34@mavs.uta.edu)

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
Revised		: 19th October, 2015
Version		: 1.0
Target uC	: TM4C123GH6PM
Clock Source: 16 MHz primary oscillator
Clock Rate	: 40 MHz using PLL
Devices used: UART

------------------------------------------------------------------------------------
Objective
------------------------------------------------------------------------------------
CONTROLLER Code:
Get command(string) from PC (RS-232), parse it and send instruction through RS-485.


-----------------------------------------------------------------------------
 Hardware Target
-----------------------------------------------------------------------------

Hardware configuration:
Red Backlight LED: PB5 drives an NPN transistor that powers the red LED
Green Backlight LED: PE4 drives an NPN transistor that powers the green LED
Blue Backlight LED: PE5 drives an NPN transistor that powers the blue LED
Red LED: PF1 drives an NPN transistor that powers the red LED
Green LED: PF3 drives an NPN transistor that powers the green LED
Pushbutton: SW1 pulls pin PF4 low (internal pull-up is used)
UART Interface:
  U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
  The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port
  Configured to 115,200 baud, 8N1
*/

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "dmx512.h"
#include "main.h"

//-----------------------------------------------------------------------------
// Global Variable
//-----------------------------------------------------------------------------
//extern unsigned char dmxData[513];			// RS485 Buffer


//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
	// Local variables
	char buffer[6];
	unsigned int deviceAdd;  			//***************** Not sure yet if this needs to be global***********//
	int deviceMode;			//***************** Not sure yet if this needs to be global***********//


	// Initialize hardware
	initHw();
    initUart0();

    // Step 1
    RED_LED = 1;
    waitMicrosecond(250000);
    RED_LED = 0;
    waitMicrosecond(250000);
    waitPbPress();				// Wait for PB press

	// Display greeting
    deviceAdd = readDevAdd();
    deviceMode = readDevMode();
    ltoa(deviceAdd,buffer);
    putsUart0("Device Address: ");
    putsUart0(buffer);
    putsUart0("\r\n");
    (deviceMode == TX_MODE) ? putsUart0("Tx Mode\r\n"):putsUart0("Rx Mode\r\n");


    while(1)
    {
    	if(deviceMode == TX_MODE)
    		getCmd();
    }
}
