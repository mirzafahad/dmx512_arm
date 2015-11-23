/*! \file uart0.h \brief UART0 functions. */
//*****************************************************************************
/*
UART0 functions

All rights reserved @
Fahad Mirza (fahad.mirza34@mavs.uta.edu)


Course Project (Fall 2015)
Embedded System Controller - EE5314
Dr. Jason Losh
Department of Electrical Engineering
University of Texas at Arlington

-----------------------------------------------------------------------------
Notes
-----------------------------------------------------------------------------
File Name	: 'uart0.h'
Created		: 19th October, 2015
Revised		: 2nd November, 2015
Version		: 1.0
Target uC	: TM4C123GH6PM
Clock Source: 16 MHz primary oscillator
Clock Rate	: 40 MHz using PLL
Devices used: UART
*/

#ifndef UART0_H_
#define UART0_H_

void initUart0();
void putcUart0(char);
void putsUart0(const char*);
char getcUart0();

#endif /* UART0_H_ */
