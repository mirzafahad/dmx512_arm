/*! \file uart1.c \brief UART function library. */
//*****************************************************************************
// Main Author: Jason Losh
// Modified by Fahad Mirza
//-----------------------------------------------------------------------------
// Objectives and notes             
//-----------------------------------------------------------------------------
// File Name	: 'uart1.c'
// Title		: UART functions
// Created		: 10th March, 2014
// Revised		: 28th April, 2014
// Version		: 1.0

// Target uC:       33FJ128MC802
// Clock Source:    8 MHz primary oscillator set in configuration bits
// Clock Rate:      80 MHz using prediv=2, plldiv=40, postdiv=2
// Devices used:    UART
//*****************************************************************************



//-----------------------------------------------------------------------------
// Device includes and assembler directives             
//-----------------------------------------------------------------------------
#include <p33FJ128MC802.h>
#include <stdio.h>
#include <string.h>
#include "uart1.h"


//-----------------------------------------------------------------------------
// Subroutines                
//-----------------------------------------------------------------------------

// Initializes UART1 with desired Baud rate
void uart1_init(int baud_rate)
{
  U1BRG = baud_rate;		// Set baud rate
  U1MODE = 0x8000;			// Enable uarts, 8N1, low speed brg
  U1STA = 0x0400;			// Enable tx, rx & Tx interrupt(00) 
  IFS0bits.U1TXIF = 0;		// Reset the interrupt flag, because when UTXEN is set, Flag bit also set
  IEC0bits.U1TXIE = 1;		// Tx Interrupt Enable 
}


// Print a string
void uart1_puts(const char str[])
{
  int i=0;
  while (str[i] != 0)
  {
    uart1_putc(str[i++]);
  }
}


//Print a character
void uart1_putc(char data)
{
   while(U1STAbits.UTXBF);	// make sure buffer is empty
   U1TXREG = data;			// write character
}


// Get a character from UART
char uart1_getc()
{
  if (U1STAbits.OERR == 1)	// clear out any overflow error condition
    U1STAbits.OERR = 0;
  
  while(!U1STAbits.URXDA);	// wait until character is ready
  return U1RXREG;
}
