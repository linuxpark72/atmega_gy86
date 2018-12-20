/*
 *  hmc5883l.h
 *
 * Desc    : 3-Axis Digital compass IC driver.
 * Created : 2018. 2.  4
 * Author  : jeho park<linuxpark@gmail.com>
 */
#ifndef _HMC5883L_
#define _HMC5883L_

// HMC5883 I2C slave address
#define HMC5883_ADDR          0x3C
#define HMC5883_WRITE         0x3C
#define HMC5883_READ          0x3D

// HMC5883 register addresses
#define HMC5883_REG_CONFIG_A  0x00
#define HMC5883_REG_CONFIG_B  0x01
#define HMC5883_REG_MODE      0x02
#define HMC5883_REG_DATAX_H   0x03
#define HMC5883_REG_DATAX_L   0x04
#define HMC5883_REG_DATAZ_H   0x05
#define HMC5883_REG_DATAZ_L   0x06
#define HMC5883_REG_DATAY_H   0x07
#define HMC5883_REG_DATAY_L   0x08
#define HMC5883_REG_SR        0x09
#define HMC5883_REG_IA        0x0A
#define HMC5883_REG_IB        0x0B
#define HMC5883_REG_IC        0x0

// HMC5883 configuration
// 3 dimensional axis
#define X_AXIS  1
#define Y_AXIS  2
#define Z_AXIS  3

// HMC5883 configuration
// update rate = 75 Hz
#define HMC_UPDATE_RATE  0x18
// measurement mode = normal (No positive/negative biasing)
#define HMC_MODE_NORMAL  0x00
// number of samples averaged per measurement output = 8
#define HMC_SAMPLE_RATE  0x60
// gain = 1090 LSb/Gauss
#define HMC_GAIN_1090  0x01
// disable high speed I2C, enable single measurement mode
//#define HMC_MODE  0x01
#define HMC_MODE  0x00
// scaling factor for gain = 1090 LSb/Gauss
#define SCALING_FACTOR  0.73
// PI value
//#define PI  3.14159265358979323846

extern int hmc5883l_test(void);

#endif /* _HMC5883L_ */
