/*
 *  i2c.c 
 *
 * Desc    : stolen from below site
 * Created : Sun Dec 10 KST 2017
 * Author  : jeho park<linuxpark@gmail.com>
 */

/*
 * modified version of I2C master library
 * added a timeout variable for non blocking i2c
 */

/*************************************************************************
* Title:    I2C master library using hardware TWI interface
* Author:   Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury
* File:     $Id: twimaster.c,v 1.3 2005/07/02 11:14:21 Peter Exp $
* Software: AVR-GCC 3.4.3 / avr-libc 1.2.3
* Target:   any AVR device with hardware TWI 
* Usage:    API compatible with I2C Software Library i2cmaster.h
**************************************************************************/
#include <inttypes.h>
#include <compat/twi.h>
#include "i2c.h"


/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
/*  jeho */
#define F_CPU 16000000UL
#endif
#include <util/delay.h>

/* I2C clock in Hz */
#define SCL_CLOCK  100000L
//#define SCL_CLOCK  400000L

/* I2C timer max delay */
#define I2C_TIMER_DELAY 0xFF

#define I2C_SCL PD0
#define I2C_SDA PD1
/*************************************************************************
 Initialization of the I2C bus interface. Need to be called only once
*************************************************************************/
void i2c_init(void)
{
  DDRC |= (1 << I2C_SCL); // SCL 핀을 출력으로 설정
  DDRC |= (1 << I2C_SDA); // SDA 핀을 출력으로 설정

  /* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
#if 0
  TWSR = 0;                         /* no prescaler */
  TWBR = ((F_CPU/SCL_CLOCK)-16)/2;  /* must be > 10 for stable operation */
#else
  TWBR = 32; // I2C 클록 주파수 설정 200KHz
#endif
  TWCR = (1 << TWEN) | (1 << TWEA); // I2C 활성화, ACK 허용

}/* i2c_init */


/*************************************************************************	
  Issues a start condition and sends address and transfer direction.
  return 0 = device accessible, 1= failed to access device
*************************************************************************/
unsigned char i2c_start(unsigned char address)
{
	uint32_t  i2c_timer = 0;
    uint8_t   twst;

	// send START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait until transmission completed
	i2c_timer = I2C_TIMER_DELAY;
	while(!(TWCR & (1<<TWINT)) && i2c_timer--);
	if(i2c_timer == 0)
		return 1;

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_START) && (twst != TW_REP_START)) return 1;

	// send device address
	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wail until transmission completed and ACK/NACK has been received
	i2c_timer = I2C_TIMER_DELAY;
	while(!(TWCR & (1<<TWINT)) && i2c_timer--);
	if(i2c_timer == 0)
		return 1;

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;

	return 0;

}/* i2c_start */


/*************************************************************************
 Issues a start condition and sends address and transfer direction.
 If device is busy, use ack polling to wait until device is ready
 
 Input:   address and transfer direction of I2C device
*************************************************************************/
void i2c_start_wait(unsigned char address)
{
	uint32_t  i2c_timer = 0;
	uint8_t   twst;

    while ( 1 )
    {
	    // send START condition
	    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    
    	// wait until transmission completed
	    i2c_timer = I2C_TIMER_DELAY;
    	while(!(TWCR & (1<<TWINT)) && i2c_timer--);

    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ( (twst != TW_START) && (twst != TW_REP_START)) continue;
    
    	// send device address
    	TWDR = address;
    	TWCR = (1<<TWINT) | (1<<TWEN);
    
    	// wail until transmission completed
    	i2c_timer = I2C_TIMER_DELAY;
    	while(!(TWCR & (1<<TWINT)) && i2c_timer--);
    
    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ( (twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK) ) 
    	{    	    
    	    /* device busy, send stop condition to terminate write operation */
	        TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	        
	        // wait until stop condition is executed and bus released
	        i2c_timer = I2C_TIMER_DELAY;
	        while((TWCR & (1<<TWSTO)) && i2c_timer--);
	        
    	    continue;
    	}
    	//if( twst != TW_MT_SLA_ACK) return 1;
    	break;
     }

}/* i2c_start_wait */


/*************************************************************************
 Issues a repeated start condition and sends address and transfer direction 

 Input:   address and transfer direction of I2C device
 
 Return:  0 device accessible
          1 failed to access device
*************************************************************************/
unsigned char i2c_rep_start(unsigned char address)
{
    return i2c_start( address );

}/* i2c_rep_start */


/*************************************************************************
 Terminates the data transfer and releases the I2C bus
*************************************************************************/
void i2c_stop(void)
{
	uint32_t  i2c_timer = 0;

    /* send stop condition */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	
	// wait until stop condition is executed and bus released
	i2c_timer = I2C_TIMER_DELAY;
	while((TWCR & (1<<TWSTO)) && i2c_timer--);

}/* i2c_stop */


/*************************************************************************
  Send one byte to I2C device
  
  Input:    byte to be transfered
  Return:   0 write successful 
            1 write failed
*************************************************************************/
unsigned char i2c_write( unsigned char data )
{	
	uint32_t  i2c_timer = 0;
    uint8_t   twst;
    
	// send data to the previously addressed device
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wait until transmission completed
	i2c_timer = I2C_TIMER_DELAY;
	while(!(TWCR & (1<<TWINT)) && i2c_timer--);
	if(i2c_timer == 0)
		return 1;

	// check value of TWI Status Register. Mask prescaler bits
	twst = TW_STATUS & 0xF8;
	if( twst != TW_MT_DATA_ACK) return 1;
	return 0;

}/* i2c_write */


/*************************************************************************
 Read one byte from the I2C device, request more data from device 
 
 Return:  byte read from I2C device
*************************************************************************/
unsigned char i2c_readAck(void)
{
	uint32_t  i2c_timer = 0;

	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	i2c_timer = I2C_TIMER_DELAY;
	while(!(TWCR & (1<<TWINT)) && i2c_timer--);
	if(i2c_timer == 0)
		return 0;

    return TWDR;

}/* i2c_readAck */


/*************************************************************************
 Read one byte from the I2C device, read is followed by a stop condition 
 
 Return:  byte read from I2C device
*************************************************************************/
unsigned char i2c_readNak(void)
{
	uint32_t  i2c_timer = 0;

	TWCR = (1<<TWINT) | (1<<TWEN);
	i2c_timer = I2C_TIMER_DELAY;
	while(!(TWCR & (1<<TWINT)) && i2c_timer--);
	if(i2c_timer == 0)
		return 0;
	
    return TWDR;

}/* i2c_readNak */

unsigned int i2c_send(char addr, char cmd)
{
    unsigned char ret;

    ret = i2c_start(addr + I2C_WRITE /* 0 */); // set device address and write mode
    if ( ret ) {
        //failed to issue start condition, possibly no device found */
        i2c_stop();
        return 0;
    } else {
        // issuing start condition ok, device accessible
        ret = i2c_write(cmd);
        i2c_stop();
        return 1;
    }
}

/*
 * read bytes from chip register
 */
int8_t i2c_readBytes(uint8_t dev, uint8_t regAddr, uint8_t length, uint8_t *data) {
    uint8_t i = 0;
    int8_t count = 0;

    if(length > 0) {
        //request register
        i2c_start(dev | I2C_WRITE);
        i2c_write(regAddr);
        _delay_us(10);
        //read data
        i2c_start(dev | I2C_READ);
        for(i=0; i<length; i++) {
            count++;
            if(i==length-1)
                data[i] = i2c_readNak();
            else
                data[i] = i2c_readAck();
        }
        i2c_stop();
    }
    return count;
}

/*
 * read 1 byte from chip register
 */
int8_t i2c_readByte(uint8_t dev, uint8_t regAddr, uint8_t *data) {
    return i2c_readBytes(dev, regAddr, 1, data);
}

/*
 * write bytes to chip register
 */
void i2c_writeBytes(uint8_t dev, uint8_t regAddr, uint8_t length, uint8_t* data) {
    if(length > 0) {
        //write data
        i2c_start(dev | I2C_WRITE);
        i2c_write(regAddr); //reg
        for (uint8_t i = 0; i < length; i++) {
            i2c_write((uint8_t) data[i]);
        }
        i2c_stop();
    }
}

/*
 * write 1 byte to chip register
 */
void i2c_writeByte(uint8_t dev, uint8_t regAddr, uint8_t data) {
    return i2c_writeBytes(dev, regAddr, 1, &data);
}

/*
 * read bits from chip register
 */
int8_t i2c_readBits(uint8_t dev, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    int8_t count = 0;
    if(length > 0) {
        uint8_t b;
        if ((count = i2c_readByte(dev, regAddr, &b)) != 0) {
            uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
            b &= mask;
            b >>= (bitStart - length + 1);
            *data = b;
        }
    }
    return count;
}

/*
 * read 1 bit from chip register
 */
int8_t i2c_readBit(uint8_t dev, uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
    uint8_t b;
    uint8_t count = i2c_readByte(dev, regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/*
 * write bit/bits to chip register
 */
void i2c_writeBits(uint8_t dev, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    if(length > 0) {
        uint8_t b = 0;
        if (i2c_readByte(dev, regAddr, &b) != 0) { //get current data
            uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
            data <<= (bitStart - length + 1); // shift data into correct position
            data &= mask; // zero all non-important bits in data
            b &= ~(mask); // zero all important bits in existing byte
            b |= data; // combine data with existing byte
            i2c_writeByte(dev, regAddr, b);
        }
    }
}

/*
 * write one bit to chip register
 */
void i2c_writeBit(uint8_t dev, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    i2c_readByte(dev, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    i2c_writeByte(dev, regAddr, b);
}

/*
 * write word/words to chip register (2 bytes)
 */
void i2c_writeWords(uint8_t dev, uint8_t regAddr, uint8_t length, uint16_t* data) {
    if(length > 0) {
        uint8_t i = 0;
        //write data
        i2c_start(dev | I2C_WRITE);
        i2c_write(regAddr); //reg
        for (i = 0; i < length * 2; i++) {
            i2c_write((uint8_t)(data[i++] >> 8)); // send MSB
            i2c_write((uint8_t)data[i]);          // send LSB
        }
        i2c_stop();
    }
}
