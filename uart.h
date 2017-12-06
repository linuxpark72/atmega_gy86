/*
 *  uart0.h
 * 
 * Created: 2017. 05. 23. (화) 17:06:47 KST
 * Author : jeho park<linuxpark@gmail.com>
 */
#ifndef _DRONE_UART0_
#define _DRONE_UART0_

#include "drone.h"
#include <avr/io.h>
#include <stdio.h>

#define  DEBUG_DRONE

extern FILE OUTPUT;
extern FILE INPUT;
extern void uart0_init(void);

#ifdef  DEBUG_DRONE
#define dprint(format, args...)  \
        printf("DEBUG: %s/%d: " format "\n\r", __FUNCTION__, __LINE__, ## args)
#else
#define dprint(format, args...)
#endif

#endif
