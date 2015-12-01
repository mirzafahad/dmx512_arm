/*! \file main.h \brief DMX512 Communication. */
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
File Name	: 'main.h'
Created		: 19th October, 2015
Revised		: 26th November, 2015
Version		: 1.1
Target uC	: TM4C123GH6PM
Clock Source: 16 MHz primary oscillator
Clock Rate	: 40 MHz using PLL
Devices used: UART

------------------------------------------------------------------------------------
Objective
------------------------------------------------------------------------------------
Current Objective: Step 1-7
Ultimate Objective CONTROLLER Code:
Get command(string) from PC (RS-232), parse it and send instruction through
RS-485.

------------------------------------------------------------------------------------
Hardware description
------------------------------------------------------------------------------------
See 'main.c'

*/

#ifndef MAIN_H_
#define MAIN_H_

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "uart0.h"
#include "uart1.h"
#include "dmx512.h"
#include "tm4c123gh6pm.h"
#include "bitband.h"

// Device Mode
#define TX_MODE 1
#define RX_MODE 0
//-----------------------------------------------------------------------------
// Global Variable
//-----------------------------------------------------------------------------

volatile unsigned int deviceAdd;


//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

//*********************************************************//
// Blocking function that returns only when SW1 is pressed //
//*********************************************************//
void waitPbPress()
{
	while(PUSH_BUTTON);
}

//**********************************************************************************//
// Approximate busy waiting (in units of microseconds), given a 40 MHz system clock //
//**********************************************************************************//
void waitMicrosecond(uint32_t us)
{
	                                            // Approx clocks per us
	__asm("WMS_LOOP0:   MOV  R1, #6");          // 1
    __asm("WMS_LOOP1:   SUB  R1, #1");          // 6
    __asm("             CBZ  R1, WMS_DONE1");   // 5+1*3
    __asm("             NOP");                  // 5
    __asm("             B    WMS_LOOP1");       // 5*3
    __asm("WMS_DONE1:   SUB  R0, #1");          // 1
    __asm("             CBZ  R0, WMS_DONE0");   // 1
    __asm("             B    WMS_LOOP0");       // 1*3
    __asm("WMS_DONE0:");                        // ---
                                                // 40 clocks/us + error
}


//*************************//
// Initialize the Hardware //
//*************************//
void initHw()
{
	// Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    // Note UART on port A must use APB
    SYSCTL_GPIOHBCTL_R = 0;

    // Enable GPIO port A,C,D,E and F.
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOC | SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOE | SYSCTL_RCGC2_GPIOF;

    // Configure LED and pushbutton pins
    GPIO_PORTA_DIR_R = 0x00;  // bits 3,6 and 7 are inputs
    GPIO_PORTA_DEN_R = 0xC8;  // enable 3,6,7 buttons
    GPIO_PORTA_PUR_R = 0xC8;  // enable internal pull-up for push button
    GPIO_PORTA_AFSEL_R &= ~(0x08);  // By default PA3 act as SPI, revert back to GPIO

    GPIO_PORTC_DIR_R = 0x40;  // bit 7 is input, bit 6 is output
    GPIO_PORTC_DEN_R = 0xC0;  // enable PC7 (push button) and PC6 (DEN)
    GPIO_PORTC_PUR_R = 0x80;  // enable pull-up

    GPIO_PORTD_DIR_R = 0x00;  // bits 2,3 and 6 are inputs
    GPIO_PORTD_DEN_R = 0x4C;  // enable 2,3,6 buttons
    GPIO_PORTD_PUR_R = 0x4C;  // enable pull-up

    GPIO_PORTE_DIR_R = 0x00;  // bits 1,2 and 3 are inputs
    GPIO_PORTE_DEN_R = 0x0E;  // enable 1,2,3 push buttons
    GPIO_PORTE_PUR_R = 0x0E;  // enable pull-up

    GPIO_PORTF_DIR_R = 0x0E;  // bits 1,2 & 3 are outputs, 4 is input
    GPIO_PORTF_DR2R_R = 0x0E; // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTF_DEN_R = 0x1E;  // enable LEDs and pushbuttons
    GPIO_PORTF_PUR_R = 0x10;  // enable internal pull-up for push button
}




#endif /* MAIN_H_ */
