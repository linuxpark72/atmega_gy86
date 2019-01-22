/*
 *  hmc5983.c
 *
 * Desc    : 3-Axis Digital compass IC driver.
 * Created : 2018. 2.  4
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
#include "hmc5983.h"

volatile uint16_t comp[3];
volatile uint32_t angle;

uint8_t hmc5983_readreg(uint8_t reg) {
	uint8_t data;

	i2c_start(HMC5983_ADDR + I2C_WRITE);
	i2c_write(reg);

	i2c_rep_start(HMC5983_READ);
	data = i2c_readNak();
	i2c_stop();
	return data;
}

void hmc5983_writereg(uint8_t reg, uint8_t data) {

	i2c_start(HMC5983_ADDR + I2C_WRITE);
	i2c_write(reg);
	i2c_write((uint8_t) data);
	i2c_stop();
}

uint16_t hmc5983_get_axis(uint8_t axis) {
	uint8_t data[2];
	uint16_t value;

	switch(axis)
	{

		case X_AXIS:
			data[0] = hmc5983_readreg(HMC5983_REG_DX_H);
			data[1] = hmc5983_readreg(HMC5983_REG_DX_L);
			value = (data[0] << 8) | data[1];
			break;

		case Y_AXIS:
			data[0] = hmc5983_readreg(HMC5983_REG_DY_H);
			data[1] = hmc5983_readreg(HMC5983_REG_DY_L);
			value = (data[0] << 8) | data[1];
			break;

		case Z_AXIS:
			data[0] = hmc5983_readreg(HMC5983_REG_DZ_H);
			data[1] = hmc5983_readreg(HMC5983_REG_DZ_L);
			value = (data[0] << 8) | data[1];
			break;
	}

	return value;
}

uint32_t hmc5983_get_field(uint8_t axis) {
  uint16_t axisVal;
  uint32_t fieldStrength;
  
  axisVal = hmc5983_get_axis(axis);
  fieldStrength = axisVal * SCALING_FACTOR;
  return fieldStrength;
}

uint32_t hmc5983_get_headangle(void) {

  /* read X axis component */
  comp[0] = hmc5983_get_axis(X_AXIS);
  /* read Y axis component */
  comp[1] = hmc5983_get_axis(Y_AXIS);
  /* read Z axis component */
  comp[2] = hmc5983_get_axis(Z_AXIS);
  /* calculate heading angle.Function atan2 defined in math.h */
  angle = (((atan2((int16_t)comp[0] ,(int16_t)comp[1])) * 180) / PI) + 180;
  
  return angle;
}


void hmc5983_init() {
  //uint8_t cfgA_data, cfgB_data;

  //cfgA_data = HMC_UPDATE_RATE | HMC_MODE_NORMAL | HMC_SAMPLE_RATE;
  //cfgB_data = HMC_GAIN_1090;
  // set update rate, measurement mode and sample rate
  //hmc5983_writereg(HMC5983_REG_CONFIG_A, cfgA_data);
  hmc5983_writereg(HMC5983_REG_CONFIG_A, 0x70);
  // set device gain
  //hmc5983_writereg(HMC5983_REG_CONFIG_B, cfgB_data);
  hmc5983_writereg(HMC5983_REG_CONFIG_B, 0xA0);
  
  /* set operating mode of the device
   * HS & single mode
   */
  hmc5983_writereg(HMC5983_REG_MODE, 0x81);
}

int hmc5983_isready(void) {
	uint8_t data;
	
	data = hmc5983_readreg(HMC5983_REG_SR);
	return data & 0x01;
}

int hmc5983_id_check() {
	uint8_t a, b, c;
	a = b = c = 0;
	
	i2c_start(HMC5983_ADDR);
	i2c_write(HMC5983_REG_IA);
	i2c_rep_start(HMC5983_READ);
	a = i2c_readAck();
	b = i2c_readAck();
	c = i2c_readNak();
	i2c_stop();
	printf("ID: 0x48 ?= 0x%x, 0x34 ?= 0x%x 0x33 ?= 0x%x\r\n", a, b, c);

	if (a != 0x48 || b != 0x34 || c != 0x33) {
		return -1;
	}

	return 0;
}

int hmc5983_test(void) {
#ifdef HMC5983_TEST

	printf("\r\nhmc5983 testing...\r\n");

	if (hmc5983_id_check() < 0) {
		printf("hmc5983: connection failed! \r\n");
		return -1;
	} else {
		printf("hmc5983: check done! \r\n");
	}

	hmc5983_init();
	_delay_ms(1000);
	while(1) {
		while(!hmc5983_isready());

		hmc5983_get_headangle();
		printf("angle(%lu), x(%u), y(%u), z(%u)\r\n", 
			   angle, comp[X_AXIS-1], comp[Y_AXIS-1], comp[Z_AXIS-1]);

	} 

	return 0;
#endif /* HMC5983_TEST */ 
}
