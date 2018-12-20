/*
 *  hmc5883l.c
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
#include "hmc5883l.h"
#define PI  3.14159265359
#define Declination -0.00669
#if 0
static unsigned int hmc5883_i2c_send(char cmd) {
	return i2c_send(HMC5883_ADDR, cmd);
}

uint8_t hmcreadRegister(uint8_t regAddr) {
  uint8_t data;

  //hmc5883_i2c_send(HMC5883_READ);

  //request register
  i2c_start(HMC5883_ADDR + i2c_READ);
  i2c_write(regAddr);
  i2c_stop();
  _delay_us(10);
  //read data
  i2c_start(HMC5883_ADDR + i2c_READ);
  data = i2c_readNak();
  i2c_stop();
  return data;
}

void hmcwriteRegister(uint8_t regAddr, uint8_t data) {
  //i2c_writeByte(HMC5883_ADDR, regAddr, data);

  //hmc5883_i2c_send(HMC5883_WRITE);

  //write data
  i2c_start(HMC5883_ADDR + i2c_WRITE);
  //i2c_write(HMC5883_WRITE); //reg
  i2c_write(regAddr); //reg
  i2c_write((uint8_t) data);
  i2c_stop();
}

uint16_t hmcGetAxisComponent(uint8_t axis) {
  uint8_t data[2];
  uint16_t value;

  switch(axis)
  {

	case X_AXIS:
	  data[0] = hmcreadRegister(HMC5883_REG_DATAX_H);
	  data[1] = hmcreadRegister(HMC5883_REG_DATAX_L);
	  value = (data[0] << 8) | data[1];
	  break;

	case Y_AXIS:
	  data[0] = hmcreadRegister(HMC5883_REG_DATAY_H);
	  data[1] = hmcreadRegister(HMC5883_REG_DATAY_L);
	  value = (data[0] << 8) | data[1];
	  break;

	case Z_AXIS:
	  data[0] = hmcreadRegister(HMC5883_REG_DATAZ_H);
	  data[1] = hmcreadRegister(HMC5883_REG_DATAZ_L);
	  value = (data[0] << 8) | data[1];
	  break;
  }

  return value;
}

uint32_t hmcGetMagneticFieldStrength(uint8_t axis) {
  uint16_t axisVal;
  uint32_t fieldStrength;
  
  // read axis component
  axisVal = hmcGetAxisComponent(axis);
  // multiply raw value by scaling factor
  fieldStrength = axisVal * SCALING_FACTOR;
  return fieldStrength;
}

uint32_t hmcGetHeadingAngle(void) {
  uint16_t comp[3];
  uint32_t angle;

  // read X axis component
  comp[0] = hmcGetAxisComponent(X_AXIS);
  // read Y axis component
  comp[1] = hmcGetAxisComponent(Y_AXIS);
  // read Z axis component
  comp[2] = hmcGetAxisComponent(Z_AXIS);
  // calculate heading angle.Function atan2 defined in math.h
  angle = (((atan2((int16_t)comp[0] ,(int16_t)comp[1])) * 180) / PI) + 180;
  
  return angle;
}


void hmcInit() {
  //uint8_t cfgA_data, cfgB_data;

  //cfgA_data = HMC_UPDATE_RATE | HMC_MODE_NORMAL | HMC_SAMPLE_RATE;
  //cfgB_data = HMC_GAIN_1090;
  
  // set update rate, measurement mode and sample rate
  //hmcwriteRegister(HMC5883_REG_CONFIG_A, cfgA_data);
  hmcwriteRegister(HMC5883_REG_CONFIG_A, 0x70);
  // set device gain
  //hmcwriteRegister(HMC5883_REG_CONFIG_B, cfgB_data);
  hmcwriteRegister(HMC5883_REG_CONFIG_B, 0xA0);
  // set operating mode of the device
  //hmcwriteRegister(HMC5883_REG_MODE, HMC_MODE);
  //hmcwriteRegister(HMC5883_REG_MODE, 0x01);
}

void hmcVerify() {
	uint8_t data;

	printf("\r\n");
	data = hmcreadRegister(HMC5883_REG_IA);
	printf("IA: 0x%x\r\n", data);
	data = hmcreadRegister(HMC5883_REG_IB);
	printf("IB: 0x%x\r\n", data);
	data = hmcreadRegister(HMC5883_REG_IC);
	printf("IC: 0x%x\r\n", data);
	data = hmcreadRegister(HMC5883_REG_SR);
	printf("SR: %x\r\n", data);
}

int hmc_isready(void) {
	uint8_t data;
	
	data = hmcreadRegister(HMC5883_REG_SR);
	return data & 0x01;
}
#endif
void Magneto_init()                    
{
    i2c_start(0x3C);
    i2c_write(0x00);
    i2c_write(0x70);
    i2c_write(0xA0);
    i2c_write(0x00);
    i2c_stop();
}

int Magneto_GetHeading()
{
	int x, y, z;
	double Heading;

	i2c_start(0x3C);
	i2c_write(0x03);
	i2c_rep_start(0x3D);
	/* read 16 bit x,y,z value (2?s complement form) */
	x = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
	z = (((int)i2c_readAck()<<8) | (int)i2c_readAck());
	y = (((int)i2c_readAck()<<8) | (int)i2c_readNak());
	i2c_stop();
	printf("x(%u), y(%u), z(%u)\r\n", x, y, z);
	Heading = atan2((double)y,(double)x) + Declination;
	if (Heading>2*PI)
		Heading = Heading - 2*PI;
	if (Heading<0) 
		Heading = Heading + 2*PI;
	return (Heading* 180 / PI);
}

void check() {
	char a, b, c;

	i2c_start(0x3C);
	i2c_write(0x0A);
	i2c_rep_start(0x3D);
	a = i2c_readAck();
	b = i2c_readAck();
	c = i2c_readNak();
	i2c_stop();

	printf("%c %c %c\r\n", a, b, c);
}

int hmc5883l_test(void) {
#ifdef HMC5883_TEST
	int ret;

	printf("\r\n");
	printf("testing...\r\n");
	Magneto_init ();
	printf("done\r\n");
	check();
	printf("v done\r\n");
	while (1) {
		ret = Magneto_GetHeading();
		printf("%u\r\n", ret);
		_delay_ms(1000);
	}
#endif /* HMC5883_TEST */
	return 1;
}
