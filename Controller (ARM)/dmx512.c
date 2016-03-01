/*! \file dmx512.c \brief DMX512 Functions. */
//*****************************************************************************
/*
DMX512 functions

All rights reserved @ Fahad Mirza (fahadmirza80@yahoo.com)


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
Revised		: 26th November, 2015
Version		: 1.1
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
#include "uart1.h"
#include "dmx512.h"
#include "bitband.h"
#include "wait.h"
#include "tm4c123gh6pm.h"


#define DEBUG 0
#define MAX_LENGTH 80		// Max number of bytes receiving from user at a time

//-----------------------------------------------------------------------------
// Constant Arrays
//-----------------------------------------------------------------------------
const char errMsg[] = "Error. Type 'help'.\r\n";
const char help[] 	= "\r\nCmds are case insensitive.\r\nAdr:1 to 512; data:0 to 255\r\n---------------------------\r\nset Adr data\r\nget Adr\r\nmax Adr\r\non\r\noff\r\npoll\r\nclear\r\n";
const char ready[]	= "Ready\r\n";

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

char inputStr[MAX_LENGTH+1];		// +1 for NULL; To hold the user cmd
volatile unsigned char dmxData[513];// Address 1 to 512 will be used
volatile char pollData[513];		// Poll Buffer
int fieldCount;						// How many fields does CMD has?
int maxDmxAddr = 512;				// Max Data slot for RS485
bool txOn = true;					// RS485 On/Off

int pollDevAddr[10];				// variables for POLL cmd
int pollDevIndex = 0;
volatile int txPhase = 0;
//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

//**********************************************//
// Read the 9 pins dip switch as device address //
//**********************************************//
int readDevAdd()
{
	unsigned int deviceAdd;

	deviceAdd = DEV_ADD8<<8 | DEV_ADD7<<7 | DEV_ADD6<<6 | DEV_ADD5<<5 | DEV_ADD4<<4 | DEV_ADD3<<3 | DEV_ADD2<<2 | DEV_ADD1<<1 | DEV_ADD0;

	deviceAdd = (~deviceAdd) & 0x1FF; // Pins are active low

	return deviceAdd;
}

//******************************************//
// Read the 10th dip switch for device mode //
//******************************************//
int readDevMode()
{
	int deviceMode;

	deviceMode = (~DEV_MODE) & 0x1;

	return deviceMode;
}

/*****************************************************************
* Parse and Find the point of interest in dmxData(Position in the
* array and type[Number, Character, delimiter]). If the CMD is
* not valid it will print(UART0) where is the error.
*
* Input	: Pointer for Position and Type
* Return: True, if the CMD is valid
*		  False, if not valid.
*****************************************************************/
bool parseStr(int pos[], char type[])
{
   bool noError = true;						// invalid cmd flag
   unsigned int i,j=0;
   unsigned char last_type = 'd';

   fieldCount = 0;							// Initialize the global variable

   for(i=0;inputStr[i] != NULL && i<=MAX_LENGTH+1; i++)	// parse till string is null or till 20 character
   {
      if(isalnum(inputStr[i]))				// Is it Alpahanumeric?
      {
	     if(isalpha(inputStr[i]))			// Is it Alphabet?
		 {
		    if(last_type == 'd')			// d -> a transition. Record the position
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


/*******************************************************************
* Compare CMD and its field count with the string and count argument
* Return: 1(matched) or 0 (didn't match)
*******************************************************************/
int isCmd(char strVerb[], int count)
{
   return ((strcmp(strVerb, inputStr)==0) && (count == fieldCount));
}


/**********************************************
* Convert the String Number from CMD to integer
*
* Input: Index of CMD(dmxData[])
* Return: The number in Int
**********************************************/
int getArgNum(int index)
{
   return (atoi(&inputStr[index]));					//Change 'addr' and 'value' from ascii to integer
}


/************************************************
* Get character from UART0 and organize the array
* Save data in Global dmxData[]
*
* Return: 1, if received CR or buffer full
*		  0, else
************************************************/
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




bool poll(int min, int max)
{
	int i,j;						// 'for loop' variables

	for(i=1;i<513;i++)				// Prepare poll data. Set '1' from min to max, '0' for the rest
	{
		if(i>=min && i<=max)
			pollData[i] = 1;		// pollData start from 1 to 512
		else
			pollData[i] = 0;
	}

	if(pollDevIndex>0)				// Did we already find any device address on the bus?
	{
		for(i=1,j=0;i<513;i++) 		// If so, check and set those address's poll data as 0
		{
			if(i == pollDevAddr[j])
			{
				pollData[i] = 0;
				j++;
				if(j>= pollDevIndex) break; // If we already clear all 'Found' devices addresses, no need to check further
			}
		}
	}

	dmxTxDEN = 1;							// Enable write mode
	waitMicrosecond(1);

	brkFunc();
	sendStartByte(pollStartByte);

	for(i=1;i<513;i++)
	{
		putcUart1(pollData[i]);				// Send all 512 data
	}

	while(U1_TX_BUSY);						// Wait till data transmit ends

	dmxTxDEN = 0;							// Enable read mode

	for(i=0;i<60;i++)						// To detect a data zero with a frame error requires 44uS, we will check for data till 60uS
	{
		if(U1_RX_EMPTY == 0)				// Is there any data?
		{
			if(getcUart1() == 0)			// Is data zero?	//if((UART1_DR_R & 0x1FF) == 0x100)
			{
				if(U1_FRAME_STATUS)			// Frame error?
				{
					//redTimeout = 250;		// Set RED LED, indicate valid break receive
					//LATBbits.LATB5 = 1;
					U1_FRAME_STATUS = 1;
					waitMicrosecond(92);	// Detect BREAK within 44us(40us in real), but BREAK & MAB send by device is for 132us (approx.)
					return true;				// Got a response, return 1
				}
			}
		}
		waitMicrosecond(1);
	}

	return false;
}


/*********************************************
* Find device's response between min and max.
* Binary search (recursively)
*******************************************/
bool findDevice(int min, int max)
{
	if(poll(min,max))
	{
		if(min == max)
		{
			pollDevAddr[pollDevIndex++] = min;
			return true;
		}
		else if(findDevice(min,(min+max)>>1))
		{
			return true;
		}
		else if(findDevice(((min+max)>>1)+1,max))
		{
			return true;
		}
	}

	return false;
}


/************************************************
* Find all device addresses available on the bus
************************************************/
void listDevice(void)
{
	while(findDevice(1,512));
	//while(findDevice(1,512));
	//while(findDevice(1,512));

	if(pollDevIndex>0)
	{
		char buffer[6];
		int i;
		putsUart0("Device(s) available on the bus:\r\n");
		for(i=0; i<pollDevIndex; i++)
		{
			ltoa(pollDevAddr[i],buffer);
			putsUart0(buffer);
			putcUart0(' ');
		}
		putsUart0("\r\n");

		pollDevIndex = 0;
	}
}

//**************************************//
// Read user data & process accordingly //
//**************************************//
void getCmd()
{
	if(getInputChar())								// Get character form UART0. If it is CR then go to next step.
	{

		int invalidCmd=0;							// Flag for invalid cmd

		if(DEBUG){ putsUart0(inputStr); putsUart0("\r\n"); }

		int pos[4];
		char type[4];

		if(parseStr(pos, type)) 					// If parsing is successful, then execute the cmd
		{
			int addr, data;

			if(isCmd("set",3))						// Is it 'SET'?
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
			else if(isCmd("get",2))					// Is it 'GET' cmd?
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
				txOn = true;						// Turn ON the DMX transmission.
				enableU1TxINT();					// Maybe I should call brkfunc() to avoid any issue in future**********************************************
			}
			else if(isCmd("off",1))					// Is it a 'OFF' cmd?
			{
				txOn = false;						// Maybe I should set txPhase to 1 and clear the flag to make things error prone proof*************************
				disableU1TxINT();					// Turn OFF the DMX transmission.
			}
			else if(isCmd("poll",1))
			{
				disableU1TxINT();
				listDevice();

				dmxTxDEN = 1;							// Enable write mode
				waitMicrosecond(1);
				txPhase = 0;
				if(txOn)
					enableU1TxINT();
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



//******************//
// Clear DMX Buffer //
//******************//
void clrDmxData()
{
	int i;
	for(i=1;i<513;i++)
	{
		dmxData[i] = 0;
	}
}




//*******************************//
// DMX Break generation function //
//*******************************//
void brkFunc()
{
	while(U1_TX_BUSY);								   // Wait till Tx FIFO is empty

	UART1_IBRD_R = 30;              				   // Slow the Baud Rate
													   // 83.33Kbps  Math: r = 40 MHz / (Nx83.33kHz) = 30, where N=16 and No fraction

	UART1_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_STP2;  // According to datasheet everytime we change baudrate we must write to LCRH
													   //configure for 8N2 w/ 1-level FIFO

	putcUart1(0);									   // Sending Break and MAB
}

//********************************//
// Start Byte generation function //
//********************************//
void sendStartByte(char startByte)
{
	while(U1_TX_BUSY);								  // Wait till Tx FIFO is empty
	UART1_IBRD_R = 10;          					  // Increase the Baud Rate
													  // 250Kbps  Math: r = 40 MHz / (Nx250kHz) = 10, where N=16 and No fraction

	UART1_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_STP2; // According to datasheet everytime we change baudrate we must write to LCRH
													  //configure for 8N2 w/ 1-level FIFO
	putcUart1(startByte);							  // Sending StartByte
}


/********************************************
* Read all 512 poll data (Blocking function)
*
* Return: Device address's data (true/false)
********************************************/
bool readPollData(int deviceAdd)
{
	int index;
	char temp;
	bool data;

	for(index=1; index<513; index++)			// Read all 512 data
	{
		temp = getcUart1();
		if(index == deviceAdd)					// Is the data for this device?
		{
			data = temp>0;
		}
	}
	//char buffer[6];
	//ltoa(data,buffer);
	//putsUart0(buffer);

	return data;
}


/*// Turn On DMX transmission
void dmxTxOn()
{
	dmxTxDEN = 1;
	txPhase = 1;
	enableU1TxINT();
}


// Turn On DMX transmission
void dmxTxOff()
{
	UART1_IM_R = 0;                       // turn-off TX interrupt
	U1_TX_INT_FLAG = 1;					  // Clear the flag
}*/
