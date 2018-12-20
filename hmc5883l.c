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
#include "i2c.h"
#include "uart.h"
#include "hmc5883l.h"
#include <math.h>

uint8_t hmcReadRegister(uint8_t regAddr) {
  uint8_t data;

  i2c_readByte(HMC5883_SLAVE_ADDR, regAddr, &data);
  return data;
}

void hmcWriteRegister(uint8_t regAddr, uint8_t data) {

  i2c_writeByte(HMC5883_SLAVE_ADDR, regAddr, data);
}

uint16_t hmcGetAxisComponent(uint8_t axis) {
  uint8_t data[2];
  uint16_t value;

  switch(axis)
  {

	case X_AXIS:
	  data[0] = hmcReadRegister(HMC5883_REG_DATAX_H);
	  data[1] = hmcReadRegister(HMC5883_REG_DATAX_L);
	  value = (data[0] << 8) | data[1];
	  break;

	case Y_AXIS:
	  data[0] = hmcReadRegister(HMC5883_REG_DATAY_H);
	  data[1] = hmcReadRegister(HMC5883_REG_DATAY_L);
	  value = (data[0] << 8) | data[1];
	  break;

	case Z_AXIS:
	  data[0] = hmcReadRegister(HMC5883_REG_DATAZ_H);
	  data[1] = hmcReadRegister(HMC5883_REG_DATAZ_L);
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
  //hmcWriteRegister(HMC5883_REG_CONFIG_A, cfgA_data);
  hmcWriteRegister(HMC5883_REG_CONFIG_A, 0x70);
  // set device gain
  //hmcWriteRegister(HMC5883_REG_CONFIG_B, cfgB_data);
  hmcWriteRegister(HMC5883_REG_CONFIG_B, 0xA0);
  // set operating mode of the device
  //hmcWriteRegister(HMC5883_REG_MODE, HMC_MODE);
  //hmcWriteRegister(HMC5883_REG_MODE, 0x01);
}

void hmcVerify() {
	uint8_t data;

	printf("\r\n");
	data = hmcReadRegister(HMC5883_REG_IA);
	printf("IA: %c\r\n", data);
	data = hmcReadRegister(HMC5883_REG_IB);
	printf("IB: %c\r\n", data);
	data = hmcReadRegister(HMC5883_REG_IC);
	printf("IC: %c\r\n", data);
	data = hmcReadRegister(HMC5883_REG_SR);
	printf("SR: %x\r\n", data);
}

int hmc_isready(void) {
	uint8_t data;
	
	data = hmcReadRegister(HMC5883_REG_SR);
	return data & 0x01;
}


/* continuous mode */
int hmc5883l_test(void) {
#ifdef HMC5883_TEST
	uint32_t angle;
	uint32_t x_fs;
	uint32_t y_fs;
	uint32_t z_fs;

	printf("hmc5883l_test\r\n");
	hmcInit();
	hmcVerify();

	return 1;

	while (1) {
  
		hmcWriteRegister(HMC5883_REG_MODE, 0x01);
		
		do {
			_delay_ms(1);
		} while(!hmc_isready());

		angle = hmcGetHeadingAngle();
		x_fs = hmcGetMagneticFieldStrength(X_AXIS);
		y_fs = hmcGetMagneticFieldStrength(Y_AXIS);
		z_fs = hmcGetMagneticFieldStrength(Z_AXIS);

		printf("angle(%u), x(%lu), y(%lu), z(%lu)\r\n", angle, x_fs, y_fs, z_fs);
		_delay_ms(200);
	}

#endif /* HMC5883_TEST */
	return 1;
}
