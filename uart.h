/*
 *  uart.h
 * 
 * Created: Sun Dec 10 KST 2017
 * Author : jeho park<linuxpark@gmail.com>
 */
#ifndef _UART_
#define _UART_

#include <stdio.h>

extern void uart_init(void);
extern void uart_transmit(char data, FILE *stream); 
extern char uart_receive(FILE *stream);

extern FILE OUTPUT;
extern FILE INPUT;

#ifdef  DEBUG
#define dprintf(format, args...)  printf(format, ## args)
#else
#define dprintf(format, args...)
#endif

#endif /* _UART_ */
