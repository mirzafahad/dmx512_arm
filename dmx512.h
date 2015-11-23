/*! \file dmx512.h \brief DMX512 Communication. */
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
File Name	: 'dmx512.h'
Created		: 2nd November, 2015
Revised		: 2nd November, 2015
Version		: 1.0
Target uC	: TM4C123GH6PM
Clock Source: 16 MHz primary oscillator
Clock Rate	: 40 MHz using PLL
Devices used: UART

*/

#ifndef DMX512_H_
#define DMX512_H_


extern unsigned char dmxData[513]; // Shared variable with main.c

#define pollCode 0xF0				// Start code for POLL
#define dataCode 0x00				// Start code for normal data

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------


unsigned int readDevAdd();
int readDevMode();
int getInputChar(void);
bool parseStr(int*, char*);
int isCmd(char*, int);
void clrDmxData(void);
void getCmd(void);
int getArgNum(int);
#endif /* DMX512_H_ */
