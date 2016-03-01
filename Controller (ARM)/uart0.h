/*! \file uart0.h \brief UART0 functions. */
//*****************************************************************************
/*
UART0 functions

All rights reserved @ Fahad Mirza (fahadmirza80@yahoo.com)

-----------------------------------------------------------------------------
Notes
-----------------------------------------------------------------------------
File Name	: 'uart0.h'
Created		: 19th October, 2015
Revised		: 26th November, 2015
Version		: 1.1
Target uC	: TM4C123GH6PM
Clock Source: 16 MHz primary oscillator
Clock Rate	: 40 MHz using PLL
*/

#ifndef UART0_H_
#define UART0_H_

// Initialize with 115200 8N1 w/ 16-level FIFO
void initUart0();

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(char);

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(const char*);

// Blocking function that returns with serial data once the buffer is not empty
// Return: The character
char getcUart0();

#endif /* UART0_H_ */
