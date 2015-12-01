/*! \file bitband.h \brief Single bit accessing */
//*****************************************************************************
/*
Access/Modify single bit in one cycle

All rights reserved @ Fahad Mirza (fahadmirza80@yahoo.com)


-----------------------------------------------------------------------------
Notes
-----------------------------------------------------------------------------
File Name	: 'bitband.h'
Created		: 26th November, 2015
Revised		: 26th November, 2015
Version		: 1.0
Target uC	: TM4C123GH6PM
*/


#ifndef BITBAND_H_
#define BITBAND_H_

#define RED_LED      	(*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4))) //PF1
#define BLUE_LED     	(*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4))) //PF2
#define GREEN_LED    	(*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4))) //PF3
#define PUSH_BUTTON  	(*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 4*4))) //PF4


#define DEV_ADD0  	 	(*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 1*4))) //PE1
#define DEV_ADD1  	 	(*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 2*4))) //PE2
#define DEV_ADD2  	 	(*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 3*4))) //PE3
#define DEV_ADD3  	 	(*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 3*4))) //PA3
#define DEV_ADD4  	 	(*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 6*4))) //PA6
#define DEV_ADD5  	 	(*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 7*4))) //PA7
#define DEV_ADD6  	 	(*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 2*4))) //PD2
#define DEV_ADD7  	 	(*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 3*4))) //PD3
#define DEV_ADD8  	 	(*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 6*4))) //PD6


#define DEV_MODE  	 	(*((volatile uint32_t *)(0x42000000 + (0x400063FC-0x40000000)*32 + 7*4))) //PC7
#define U1_TX_INT_FLAG  (*((volatile uint32_t *)(0x42000000 + (0x4000D044-0x40000000)*32 + 5*4))) //UARTICR TXIC bit
#define U1_FR_INT_FLAG  (*((volatile uint32_t *)(0x42000000 + (0x4000D044-0x40000000)*32 + 7*4))) //UARTICR FEIC bit
#define U1_TX_BUSY 	 	(*((volatile uint32_t *)(0x42000000 + (0x4000D018-0x40000000)*32 + 3*4))) //UART Flag register's bit 3 (Tx Busy)
#define U1_TX_INT		(*((volatile uint32_t *)(0x42000000 + (0x4000D040-0x40000000)*32 + 5*4))) //UARTMIS TXMIS bit
#define U1_RX_INT 	 	(*((volatile uint32_t *)(0x42000000 + (0x4000D040-0x40000000)*32 + 4*4))) //UARTMIS RXMIS bit
#define U1_FRAME_INT 	(*((volatile uint32_t *)(0x42000000 + (0x4000D040-0x40000000)*32 + 7*4))) //UARTMIS FEMIS bit



#define dmxTxDEN  	 (*((volatile uint32_t *)(0x42000000 + (0x400063FC-0x40000000)*32 + 6*4))) //PC6 ->RX485 Tx Enable pin



#endif /* BITBAND_H_ */
