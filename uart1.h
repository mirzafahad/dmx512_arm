/*! \file uart1.h \brief UART1 functions. */
//*****************************************************************************
/*
UART1 functions

All rights reserved @ Fahad Mirza (fahadmirza80@yahoo.com)

-----------------------------------------------------------------------------
Notes
-----------------------------------------------------------------------------
File Name	: 'uart1.h'
Created		: 23rd November, 2015
Revised		: 26th November, 2015
Version		: 1.1
Target uC	: TM4C123GH6PM
Clock Source: 16 MHz primary oscillator
Clock Rate	: 40 MHz using PLL
*/

#ifndef UART1_H_
#define UART1_H_

// Initialize UART1 with 250k 8N2 w/ UART1 Interrupt enable and 1-level FIFO
void initUart1(void);

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart1(char);

// Blocking function that writes a string when the UART buffer is not full
void putsUart1(const char*);

// Blocking function that returns with serial data once the buffer is not empty
// Returns: The character
char getcUart1(void);

// Enable Tx Interrupt
void enableU1TxINT(void);


// Disable Tx Interrupt
void disableU1TxINT(void);

#endif /* UART1_H_ */
