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
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "uart0.h"
#include "dmx512.h"
#include "tm4c123gh6pm.h"


#define DEBUG 0
#define MAX_LENGTH 80		// Max number of bytes receiving from user at a time

#define DEV_ADD0  	 (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 1*4))) //PE1
#define DEV_ADD1  	 (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 2*4))) //PE2
#define DEV_ADD2  	 (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 3*4))) //PE3
#define DEV_ADD3  	 (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 3*4))) //PA3
#define DEV_ADD4  	 (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 6*4))) //PA6
#define DEV_ADD5  	 (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 7*4))) //PA7
#define DEV_ADD6  	 (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 2*4))) //PD2
#define DEV_ADD7  	 (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 3*4))) //PD3
#define DEV_ADD8  	 (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 6*4))) //PD6
#define DEV_ADD9  	 (*((volatile uint32_t *)(0x42000000 + (0x400063FC-0x40000000)*32 + 7*4))) //PC7


//-----------------------------------------------------------------------------
// Constant Arrays
//-----------------------------------------------------------------------------
const char errMsg[] = "Error. Type 'help'.\r\n";
const char help[] = "\r\nCmds are case insensetive.\r\nAdr:1 to 512; data:0 to 255\r\n---------------------------\r\nset Adr data\r\nget Adr\r\nmax Adr\r\non\r\noff\r\npoll\r\nclear\r\n";
const char ready[] = "Ready\r\n";

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
char inputStr[MAX_LENGTH+1];// +1 for NULL; To hold the user cmd
int fieldCount;
int maxDmxAddr = 512;		// Max Data slot for RS485
int dmxOn = 1;				// RS485 On/Off
unsigned char dmxData[513];


//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

//**********************************************//
// Read the 10 pin dip switch as device address //
//**********************************************//
unsigned int readDevAdd()
{
	unsigned int deviceAdd;

	deviceAdd = DEV_ADD9<<9 |DEV_ADD8<<8 | DEV_ADD7<<7 | DEV_ADD6<<6 | DEV_ADD5<<5 | DEV_ADD4<<4 | DEV_ADD3<<3 | DEV_ADD2<<2 | DEV_ADD1<<1 | DEV_ADD0;

	deviceAdd = (~deviceAdd) & 0x3FF; // Pins are active low

	return deviceAdd;
}


//**************************************//
// Read user data & process accordingly //
//**************************************//
void getCmd()
{
	if(getInputChar())					// Get character form UART0. If it is CR then go to next step.
	{
		int invalidCmd=0;				// Flag for invalid cmd

		if(DEBUG){ putsUart0(inputStr); putsUart0("\r\n"); }

		int pos[4];
		char type[4];

		if(parseStr(pos, type)) 		// If parsing is successful, then execute the cmd
		{
			int addr, data;

			if(isCmd("set",3))			// Is it 'SET'?
			{
				if(type[1]=='n' && type[2]=='n')	// Are both of the parameters number?
				{
					addr = getArgNum(pos[1]);
					data = getArgNum(pos[2]);
					if((addr>=1 && addr<=512) && (data>=0 && data<=255)) // Check if the parameters are within range
						dmxData[addr] = data;
					else
						invalidCmd = 1;				// Out of range
				}
				else invalidCmd = 1;				// Both parameters aren't number. Error.
			}
			else if(isCmd("get",2))		// Is it 'GET' cmd?
			{
				if(type[1]=='n')					// Is the parameter a number?
				{
					addr = getArgNum(pos[1]);
					if((addr>=1 && addr<=512))		// Check if the parameter is within range
					{
						char buffer[6];
						ltoa(dmxData[addr],buffer);
						putsUart0(buffer);
						putsUart0("\r\n");
					}
					else invalidCmd = 1;			// Out of range
				}
				else invalidCmd = 1;				// Parameter is not a number
			}
			else if(isCmd("max",2))					// Is it a 'MAX' cmd?
			{
				if(type[1]=='n')
				{
					addr = getArgNum(pos[1]);
					if((addr>=1 && addr<=512)) 		// Check if the parameter is within range
						maxDmxAddr = addr;
					else
						invalidCmd = 1;
				}
				else invalidCmd = 1;
			}
			else if(isCmd("clear",1))				// Is it a 'CLEAR' cmd?
			{
				clrDmxData();
			}
			else if(isCmd("on",1))					// Is it a 'ON' cmd?
			{
				dmxOn = 1;							// Turn ON the DMX transmission.
			}
			else if(isCmd("off",1))					// Is it a 'OFF' cmd?
			{
				dmxOn = 0;							// Turn OFF the DMX transmission.
			}
			else if(isCmd("poll",1))
			{
				//pollFlag = 1;						// Set the pollFlag, and execute after the end of ongoing DMX transmission.
			}
			else if(isCmd("help",1))
			{
				putsUart0(help);					// Send "HELP" string.
			}
			else
				invalidCmd = 1;						// If nothing matches, certainly it is a invalid CMD
		}
		else
		{
			invalidCmd = 1;							// parseStr return error!
		}

		if(invalidCmd)								// If the cmd is invalid send error msg.
		{
			putsUart0(errMsg);
			invalidCmd = 0;
		}
		else
		{
			putsUart0(ready);
		}
	}
}


//***********************************************//
// Get character from UART and organize the array//		Step3
//***********************************************//
int getInputChar()
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
	  return 1;							// Return 1 if it is CR
   }
   else if(temp>=32 && temp<=126)		// Is it a 'Printable character'?
   {
	  inputStr[count++]=tolower(temp);	// Store with lower case
      if(count == MAX_LENGTH)			// Is buffer full?
	  {
		 inputStr[count] = NULL;
		 count = 0;
		 return 1;						// Buffer full, return 1
   	  }
   }
   return 0;							// Return 0 if it isn't CR
}


//***********************//
// Parse the CMD string //
//***********************//
bool parseStr(int pos[], char type[])
{
   bool noError = true;								// invalid cmd flag
   unsigned int i,j=0;
   unsigned char last_type = 'd';

   fieldCount = 0;								// Initialize the global variable

   for(i=0;inputStr[i] != NULL && i<=MAX_LENGTH+1; i++)	// parse till string is null or till 20 character
   {
      if(isalnum(inputStr[i]))					// Is it Alpahanumeric?
      {
	     if(isalpha(inputStr[i]))				// Is it Alphabet?
		 {
		    if(last_type == 'd')				// d -> a transition. Record the position
			{
			   pos[j] = i;
			   type[j++] = 'a';
			   last_type = 'a';
			   fieldCount++;
			}
			else if(last_type == 'n')		// n -> a tansition. Error.
			{
				noError = false;
				break;
			}
		 }
         else if(isdigit(inputStr[i]))		// Is it Number?
		 {
		    if(last_type == 'd')			// d -> n transition. Record the position.
			{
			   pos[j] = i;
			   type[j++] = 'n';
			   last_type = 'n';
			   fieldCount++;
			}
			else if(last_type == 'a')		// a -> n transition. Error.
			{
				noError = false;
				break;
			}
		 }
	  }
	  else									// If it is not Alphanumeric then it is delimiter. Replace with NULL.
	  {
		 inputStr[i] = NULL;
	     last_type = 'd';
	  }
   }
   if(!noError)
   {
	   int j;
	   for(j=0;j<i;j++)
	   {
		   putcUart0(' ');
	   }
	   putsUart0("^\r\n");
   }
   return noError;							// Return 0 if there is invalid sequence. Otherwise return 1.
}


int isCmd(char strVerb[], int count) // Check user string with predefined command. Return 1 if it match. Zero otherwise.
{
   return ((strcmp(strVerb, inputStr)==0) && (count == fieldCount));
}


//********************//
// atoi for the inStr //
//********************//
int getArgNum(int index)
{
   return (atoi(&inputStr[index]));	//Change 'addr' and 'value' from ascii to integer
}

//******************//
// Clear DMX Buffer //
//******************//
void clrDmxData()
{
	int i;
	for(i=0;i<513;i++)
	{
		dmxData[i] = 0;
	}
}

