/*! \file uart1.c \brief UART1 function definition. */
//***************************************************
/*
All rights reserved @
Fahad Mirza (fahadmirza80@yahoo.com)

-----------------------------------------------------------------------------
Notes
-----------------------------------------------------------------------------
File Name	: 'uart1.c'
Created		: 23rd November, 2015
Revised		: 26th November, 2015
Version		: 1.1
Target uC	: TM4C123GH6PM
Clock Source: 16 MHz primary oscillator
Clock Rate	: 40 MHz using PLL
*/



#include <stdint.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "uart1.h"



//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initUart1()
{
	// Enable PORT C
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC;			 			 // Tx1->PC5  Rx1->PC4
	// Configure UART1 pins
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;         			 // turn-on UART1, leave other uarts in same status
	GPIO_PORTC_AFSEL_R |= 0x30;                         		 // Enable alternate function
	GPIO_PORTC_PCTL_R &= ~(GPIO_PCTL_PC5_M | GPIO_PCTL_PC4_M); 	 // Clear config of PC4 and PC5
    GPIO_PORTC_PCTL_R |= GPIO_PCTL_PC5_U1TX | GPIO_PCTL_PC4_U1RX;// Set PC4 and PC5 as Rx and Tx
    GPIO_PORTC_DEN_R |= 0x30;                        			 // Enable digital function of PC4 and PC5

   	// Configure UART1 to 250K baud, 8N2 format
    UART1_CTL_R = 0;                                 			 // turn-off UART1 to allow safe programming
	UART1_CC_R = UART_CC_CS_SYSCLK;                  			 // use system clock (40 MHz)
    UART1_IBRD_R = 10;                               			 // 250Kbps  Math: r = 40 MHz / (Nx250kHz) = 10, where N=16
    UART1_FBRD_R = 00;                               			 // No fraction
    UART1_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_STP2;			 // configure for 8N2 w/ 1-level FIFO
    UART1_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN | UART_CTL_EOT; // enable TX, RX, UART1 and set interrupt for 'End of transmission'

    //Configure Tx Interrupt
    //UART1_IM_R = UART_IM_TXIM;					 			 // Used in separate function
    UART1_ICR_R = 0x20;;							  			 // Tx interrupt flag clear
    NVIC_EN0_R |= 1 << (INT_UART1-16);                			 // turn-on tx interrupt (22) (UART1)
}

//*********************//
// Enable Tx Interrupt //
//*********************//
void enableU1TxINT()
{
	UART1_IM_R = UART_IM_TXIM;            // turn-on U1TX interrupt
}


//**********************//
// Disable Tx Interrupt //
//**********************//
void disableU1TxINT()
{
	UART1_IM_R = 0;                       // turn-off TX interrupt
}


//***********************************************************************************//
// Blocking function that writes a serial character when the UART buffer is not full //
//***********************************************************************************//
void putcUart1(char c)
{
	while (UART1_FR_R & UART_FR_TXFF);
	UART1_DR_R = c;
}


//*************************************************************************//
// Blocking function that writes a string when the UART buffer is not full //
//*************************************************************************//
void putsUart1(const char* str)
{
	int i;
    for (i = 0; i < strlen(str); i++)
	  putcUart1(str[i]);
}


//******************************************************************************//
// Blocking function that returns with serial data once the buffer is not empty //
//******************************************************************************//
char getcUart1()
{
	while (UART1_FR_R & UART_FR_RXFE);
	return UART1_DR_R & 0xFF;
}

