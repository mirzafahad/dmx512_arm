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


/************************************************
* Get character from UART0 and organize the array
* Save data in Global dmxData[]
*
* Return: 1, if received CR or buffer full
*		  0, else
************************************************/
int getInputChar(void);


/*****************************************************************
* Parse and Find the point of interest in dmxData(Position in the
* array and type[Number, Character, delimiter]). If the CMD is
* not valid it will print(UART0) where is the error.
*
* Input	: Pointer for Position and Type
* Return: True, if the CMD is valid
*		  False, if not valid.
*****************************************************************/
bool parseStr(int*, char*);


/*******************************************************************
* Compare CMD and its field count with the string and count argument
* Return: 1(matched) or 0 (didn't match)
*******************************************************************/
int isCmd(char*, int);


/******************
* Clear dmxData[]
******************/
void clrDmxData(void);



/**********************
* Read CMD and Executes
**********************/
void getCmd(void);


/**********************************************
* Convert the String Number from CMD to integer
* Input: Index of CMD(dmxData[])
* Return: The number in Int
**********************************************/
int getArgNum(int);


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
