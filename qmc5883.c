/*
 *  qmc5883.c
 *
 * Desc    : 3-Axis Digital compass IC driver.
 * Created : 2018. 12.26
 * Author  : jeho park<linuxpark@gmail.com>
 *
 */
#define F_CPU 16000000L
#include <util/delay.h>
#include <math.h>
#include <avr/io.h>
#include <stdlib.h>
#include "i2c.h"
#include "uart.h"
#include "qmc5883.h"
#define PI  3.14159265359
#define Declination -0.00669

uint8_t qmc_address = QMC5883_ADDR;
/*
 *   QMC5883L.cpp - QMC5883L library
 *   Copyright (c) 2017 e-Gizmo Mechatronix Central
 *   Rewritten by Amoree.  All right reserved.
 *   July 10,2017
 *
 *   SET continuous measurement mode
 *   OSR = 512
 *   Full Scale Range = 8G(Gauss)
 *   ODR = 200HZ
 * 
 */

void qmc_setaddress(uint8_t addr){
	qmc_address = addr;
}

unsigned char qmc_writereg(uint8_t reg, uint8_t val){
	unsigned char ret;
#if 0
	Wire.beginTransmission(address); //Start
	Wire.write(Reg); // To tell the QMC5883L to get measures continously
	Wire.write(val); //Set up the Register
	Wire.endTransmission();
#else
	ret = i2c_start(QMC5883_ADDR + I2C_WRITE);
	if (ret)
		return 100;
	ret = i2c_write(reg); //reg
	if (ret)
		return 2;
	ret = i2c_write(val); //reg
	if (ret)
		return 3;
	i2c_stop();
	return 0;
#endif
}

unsigned char qmc_softreset() {
	return qmc_writereg(0x0A, 0x80);
}

unsigned char qmc_setmode(uint16_t mode, uint16_t odr, uint16_t rng, uint16_t osr){
	return qmc_writereg(0x09, mode|odr|rng|osr);
}

unsigned char qmc_init() {
	unsigned char ret;
	
	ret = qmc_writereg(0x0B, 0x01);
	if (ret)
		return ret;
	ret = qmc_setmode(Mode_Continuous, ODR_200Hz, RNG_8G, OSR_512);
	if (ret)
		return ret;

	return 0;
}

unsigned char qmc_readxyz(uint16_t* x,uint16_t* y,uint16_t* z) {
	unsigned char ret;
#if 0
	Wire.beginTransmission(address);
	Wire.write(0x00);
	Wire.endTransmission();
	Wire.requestFrom(address, 6);
	*x = Wire.read(); //LSB  x
	*x |= Wire.read() << 8; //MSB  x
	*y = Wire.read(); //LSB  z
	*y |= Wire.read() << 8; //MSB z
	*z = Wire.read(); //LSB y
	*z |= Wire.read() << 8; //MSB y
#else
	ret = i2c_start(QMC5883_ADDR + I2C_WRITE);
	if (ret)
		return ret;
	ret = i2c_write(0x0);
	if (ret)
		return ret;
	i2c_stop();

	ret = i2c_rep_start(QMC5883_ADDR + I2C_READ);
	if (ret)
		return ret;
	*x = i2c_readAck();
	*x |= i2c_readAck() << 8;
	*y = i2c_readAck();
	*y |= i2c_readAck() << 8;
	*z = i2c_readAck();
	*z |= i2c_readNak() << 8;
	i2c_stop();

	return 0;
#endif
}

#define I2C_SCL PD0
#define I2C_SDA PD1

int qmc5883_test() {
#ifdef QMC5883_TEST
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t z = 0;
	uint8_t id = 0;
	float heading;
	float declinationAngle = 0.0404;
	float headingDegrees = 0;
	unsigned char ret = 0;

	/* i2c in/out port */
	DDRD |= (1 << I2C_SCL); // SCL 핀을 출력으로 설정
	DDRD |= (1 << I2C_SDA); // SDA 핀을 출력으로 설정

	//PORTD |= (1 << I2C_SCL); // SCL 핀을 출력으로 설정
	//PORTD |= (1 << I2C_SDA); // SDA 핀을 출력으로 설정

#if 0
	ret = qmc_softreset();
	if (ret) {
		printf("\r\nreset error: %d\r\n", ret);
		return ret;
	}
#endif
	ret = qmc_init();
	if (ret) {
		printf("\r\ninit error: %d\r\n", ret);
		return ret;
	}
	i2c_readByte(QMC5883_ADDR, 0xd, &id);
	printf("\r\nid: %x\r\n", id);
	if (id != 0xFF) {
		return -1;
	}
	_delay_ms(1000);

	for(;;) {
		qmc_readxyz(&x, &y, &z);
		heading = atan2(y, x);

		heading += declinationAngle;
		// Find yours here: http://www.magnetic-declination.com/
		if(heading < 0)
			heading += 2*PI;
		// Check for wrap due to addition of declination.
		if(heading > 2*PI)
			heading -= 2*PI;

		headingDegrees = heading * 180/M_PI; 

		printf("x(%u), y(%u), z(%u), heading(%f), radius(%f)\r\n", 
			   x, y, z, heading, headingDegrees);

		_delay_ms(200);
	}
#endif
	return 0;
}
