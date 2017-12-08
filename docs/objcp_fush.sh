#!/bin/sh
#
############################################
prog_name=$1
dev_name=/dev/tty.usbserial-DN018JNZ

avr-objcopy -j .text -j .data -O binary ${prog_name}.elf  ${prog_name}.bin
avrdude -p atmega128 -c stk500 -P ${dev_name} -U flash:w:${prog_name}.bin:r 
