/*! \file dmx512.h \brief DMX512 functions. */
//*****************************************************************************
/*
DMX512 functions

All rights reserved @ Fahad Mirza (fahadmirza80@yahoo.com)

-----------------------------------------------------------------------------
Notes
-----------------------------------------------------------------------------
File Name	: 'dmx512.h'
Created		: 2nd November, 2015
Revised		: 26th November, 2015
Version		: 1.1
Target uC	: TM4C123GH6PM
Clock Source: 16 MHz primary oscillator
Clock Rate	: 40 MHz using PLL
Devices used: UART1

*/

#ifndef DMX512_H_
#define DMX512_H_

// Shared variables with main.c
extern unsigned char dmxData[513];
extern int maxDmxAddr;
extern int txPhase;


#define pollStartByte 0xF0				// Start code for POLL
#define dataStartByte 0x00				// Start code for normal data



//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

/*********************************************
* Read the 9 pin dip switch as device address
*********************************************/
unsigned int readDevAdd();

/*********************************************
* Read the 10th dip switch for device mode
* Return: 1(Tx_mode) or 0(Rx_mode)
*********************************************/
int readDevMode();

/******************
* Clear dmxData[]
******************/
void clrDmxData(void);

/***************************
* Read user CMD and Executes
***************************/
void getCmd(void);

/************************
 * Generate Break signal.
************************/
void brkFunc();

/*******************
 * Send Start Byte.
 * Input: Startcode
*******************/
void sendStartByte(char);



void dmxTxOn(void);
void dmxTxOff(void);

#endif /* DMX512_H_ */
