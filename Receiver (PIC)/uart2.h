/*! \file uart2.h \brief UART function library. */
//*****************************************************************************
//
// File Name	: 'uart2.h'
// Title		: UART functions
// Author		: Fahad Mirza - Copyright (C) 2014
// Created		: 5th April, 2014
// Revised		: 
// Version		: 1.0
// Target uC	: 33FJ128MC802
//*****************************************************************************

#ifndef __UART2_H__
 #define __UART2_H__


//Functions
void uart2_init(int baud_rate);
void uart2_puts(const char str[]);
void uart2_putc(char data);
unsigned char uart2_getc();

#endif
