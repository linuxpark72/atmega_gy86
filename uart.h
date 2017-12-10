/*
 *  uart.h
 * 
 * Created: Sun Dec 10 KST 2017
 * Author : jeho park<linuxpark@gmail.com>
 */
#ifndef _UART_
#define _UART_

#include <avr/io.h>
#include <stdio.h>

extern FILE OUTPUT;
extern FILE INPUT;
extern void uart_init(void);

#ifdef  DEBUG
#define dprint(format, args...)  \
        printf("(%s/%d): " format "\n\r", __FUNCTION__, __LINE__, ## args)
#else
#define dprint(format, args...)
#endif

#endif
