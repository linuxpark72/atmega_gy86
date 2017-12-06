/*
 *  uart.h
 * 
 * Created: 2017년 12월  7일 목요일 01시 20분 40초 KST
 * Author : jeho park<linuxpark@gmail.com>
 */
#ifndef _UART_
#define _UART_

#include <avr/io.h>
#include <stdio.h>

#define  DEBUG_DRONE

extern FILE OUTPUT;
extern FILE INPUT;
extern void uart_init(void);

#ifdef  DEBUG_DRONE
#define dprint(format, args...)  \
        printf("DEBUG: %s/%d: " format "\n\r", __FUNCTION__, __LINE__, ## args)
#else
#define dprint(format, args...)
#endif

#endif
