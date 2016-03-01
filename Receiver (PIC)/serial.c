// Main Author: Dr. Jason Losh
// Modified by: Fahad Mirza

//-----------------------------------------------------------------------------
// Objectives and notes             
//-----------------------------------------------------------------------------

// Using remappable pins
// Using uart engine

// Target Platform: 33FJ128MC802 Serial Demo Board
// Target uC:       33FJ128MC802
// Clock Source:    8 MHz primary oscillator set in configuration bits
// Clock Rate:      80 MHz using prediv=2, plldiv=40, postdiv=2
// Devices used:    UART

// Hardware description:
// Red LED
//   anode connected through 100ohm resistor to RB5 (pin 14), cathode grounded
// Green LED
//   anode connected through 100ohm resistor to RB4 (pin 11), cathode grounded



//-----------------------------------------------------------------------------
// Device includes and assembler directives             
//-----------------------------------------------------------------------------

#include <p33FJ32MC202.h>
#include <stdio.h>
#include <string.h>
#include "serial.h"



//-----------------------------------------------------------------------------
// Subroutines                
//-----------------------------------------------------------------------------

void serial_init(int baud_rate)
{
  // set baud rate
  U1BRG = baud_rate;
  // enable uarts, 8N1, low speed brg
  U1MODE = 0x8000;
  // enable tx and rx
  U1STA = 0x0400;
}

void serial_puts(char str[])
{
  int i;
  for (i = 0; i < strlen(str); i++)
  {
    // make sure buffer is empty
    while(U1STAbits.UTXBF);
    // write character
    U1TXREG = str[i];
  }
}

char serial_getc()
{
  // clear out any overflow error condition
  if (U1STAbits.OERR == 1)
    U1STAbits.OERR = 0;
  // wait until character is ready
  while(!U1STAbits.URXDA);
  return U1RXREG;
}

