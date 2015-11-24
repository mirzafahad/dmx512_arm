/*! \file uart1.h \brief UART0 functions. */
//*****************************************************************************
/*
UART1 functions

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
File Name	: 'uart1.h'
Created		: 23rd November, 2015
Revised		: 23rd November, 2015
Version		: 1.0
Target uC	: TM4C123GH6PM
Clock Source: 16 MHz primary oscillator
Clock Rate	: 40 MHz using PLL
Devices used: UART1
*/

#ifndef UART1_H_
#define UART1_H_

void initUart1(void);
void putcUart1(char);
void putsUart1(const char*);
char getcUart1(void);
void enableU1TxINT(void);
void disableU1TxINT(void);

#endif /* UART1_H_ */
