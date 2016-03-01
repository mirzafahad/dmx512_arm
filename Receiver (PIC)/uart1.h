/*! \file uart1.h \brief UART function library. */
//*****************************************************************************
//
// File Name	: 'uart1.h'
// Title		: UART functions
// Author		: Fahad Mirza - Copyright (C) 2014
// Created		: 10th March, 2014
// Revised		: 28th April 2014
// Version		: 1.0
// Target uC	: 33FJ128MC802
//*****************************************************************************

#ifndef __UART1_H__
 #define __UART1_H__


//Functions
void uart1_init(int baud_rate);
void uart1_puts(const char str[]);
void uart1_putc(char data);
char uart1_getc();

#endif
