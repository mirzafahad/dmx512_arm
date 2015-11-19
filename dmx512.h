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

#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "tm4c123gh6pm.h"



#define DEV_ADD0  	 (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 1*4))) //PE1
#define DEV_ADD1  	 (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 2*4))) //PE2
#define DEV_ADD2  	 (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 3*4))) //PE3
#define DEV_ADD3  	 (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 3*4))) //PA3
#define DEV_ADD4  	 (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 6*4))) //PA6
#define DEV_ADD5  	 (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 7*4))) //PA7
#define DEV_ADD6  	 (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 2*4))) //PD2
#define DEV_ADD7  	 (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 3*4))) //PD3
#define DEV_ADD8  	 (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 6*4))) //PD6
#define DEV_ADD9  	 (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 7*4))) //PD7

#define MAX_LENGTH 80		// Max number of bytes receiving from user at a time

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------


unsigned int readDevAdd();
int getInputChar(char inputStr[]);
unsigned int parseStr();
void getCmd(char inputStr[]);

#endif /* DMX512_H_ */
