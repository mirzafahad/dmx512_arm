/*! \file dmx512.c \brief DMX512 Communication. */
//*****************************************************************************
/*
DMX512 Communication

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
File Name	: 'dmx512.c'
Created		: 2nd November, 2015
Revised		: 2nd November, 2015
Version		: 1.0
Target uC	: TM4C123GH6PM
Clock Source: 16 MHz primary oscillator
Clock Rate	: 40 MHz using PLL
*/


#include "uart0.h"
#include "dmx512.h"


// Blocking function that returns only when SW1 is pressed
unsigned int readDevAdd()
{
	unsigned int deviceAdd;
	deviceAdd = DEV_ADD9;

	deviceAdd = DEV_ADD9<<9 | DEV_ADD8<<8 | DEV_ADD7<<7 | DEV_ADD6<<6 | DEV_ADD5<<5 | DEV_ADD4<<4 | DEV_ADD3<<3 | DEV_ADD2<<2 | DEV_ADD1<<1 | DEV_ADD0;

	deviceAdd = (~deviceAdd) & 0x3FF; // Pins are active low

	return deviceAdd;
}

//***********************************************//
// Get character from UART and organize the array//		Step3
//***********************************************//
int getInputChar(char inputStr[])
{
   char temp;
   static unsigned int count = 0;		// Limit user input within 80 character

   temp = getcUart0();					// Get Character
   if((temp == 8) && (count>0))			// Is it 'backspace' and does it has something to erase?
   {
      count--;
   }
   else if(temp == '\r')				// Is it CR?
   {
      inputStr[count] = NULL;			// Complete the buffer
	  count = 0;
	  return 1;
   }
   else if(temp>=32 && temp<=126)		// Is it a 'Printable character'?
   {
	  inputStr[count++]=tolower(temp);	// Store with lower case
      if(count == MAX_LENGTH)			// Is buffer full?
	  {
		 inputStr[count] = NULL;
		 count = 0;
		 return 1;
   	  }
   }
   return 0;							// Return 0 if it isn't CR
}


//***********************//
// Parse the CMD string //
//***********************//
unsigned int parseStr()
{
/*   unsigned int errorFlag=0;			// invalid cmd flag
   unsigned int j=0;
   unsigned char last_type = 'd';

   field_count = 0;						// Initialize the global variable

   for(int i=0;inStr[i] != NULL && i<=20; i++)	// parse till string is null or till 20 character
   {
      if(isalnum(inStr[i]))				// Is it Alpahanumeric?
      {
	     if(isalpha(inStr[i]))			// Is it Alphabet?
		 {
		    if(last_type == 'd')		// d -> a transition. Record the position
			{
			   pos[j] = i;
			   type[j++] = 'a';
			   last_type = 'a';
			   field_count++;
			}
			else if(last_type == 'n')	// n -> a tansition. Error.
			{
			   errorFlag = 1;
			   break;
			}
		 }
         else if(isdigit(inStr[i]))		// Is it Number?
		 {
		    if(last_type == 'd')		// d -> n transition. Record the position.
			{
			   pos[j] = i;
			   type[j++] = 'n';
			   last_type = 'n';
			   field_count++;
			}
			else if(last_type == 'a')	// a -> n transition. Error.
			{
			   errorFlag = 1;
			   break;
			}
		 }
	  }
	  else					// If it is not Alphanumeric then it is delimiter. Replace with NULL.
	  {
		 inStr[i] = NULL;
	     last_type = 'd';
	  }
   }
   return errorFlag;			// Return 1 if there is invalid sequence. Otherwise return zero.
*/}




//**************************************//
// Read user data & process accordingly //
//**************************************//
void getCmd(char inputStr[])
{
	if(getInputChar(inputStr))				// Get character form UART0. If it is CR then go to next step.
	{
		putsUart0(inputStr);
		putsUart0("\r\n");
	}
}

