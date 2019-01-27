/*
 *  hmc5983l.h
 *
 * Desc    : 3-Axis Digital compass IC driver.
 * Created : 2018. 2.  4
 * Author  : jeho park<linuxpark@gmail.com>
 */
#ifndef _HMC5983_
#define _HMC5983_

// HMC5983 I2C slave address
#define HMC5983_ADDR          0x3C
#define HMC5983_WRITE         0x3C
#define HMC5983_READ          0x3D

// HMC5983 register addresses
#define HMC5983_REG_CONFIG_A  0x00
#define HMC5983_REG_CONFIG_B  0x01
#define HMC5983_REG_MODE      0x02
#define HMC5983_REG_DX_H      0x03
#define HMC5983_REG_DX_L      0x04
#define HMC5983_REG_DZ_H      0x05
#define HMC5983_REG_DZ_L      0x06
#define HMC5983_REG_DY_H      0x07
#define HMC5983_REG_DY_L      0x08
#define HMC5983_REG_SR        0x09
#define HMC5983_REG_IA        0x0A
#define HMC5983_REG_IB        0x0B
#define HMC5983_REG_IC        0x0C
#define HMC5983_REG_DT_H      0x31
#define HMC5983_REG_DT_L      0x32

// HMC5983 configuration
// 3 dimensional axis
#define X_AXIS  0
#define Y_AXIS  1
#define Z_AXIS  2

// HMC5983 configuration
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
#define PI  3.14159265359
#define Declination -0.00669

extern uint8_t hmc5983_readreg(uint8_t reg);
extern void hmc5983_writereg(uint8_t reg, uint8_t data); 
extern uint16_t hmc5983_get_axis(uint8_t axis);
extern uint32_t hmc5983_get_field(uint8_t axis);
extern uint32_t hmc5983_get_headangle(void);
extern int hmc5983_isready(void);
extern int hmc5983_id_check(void);
extern void hmc5983_init(void);
extern int hmc5983_test(void);

#endif /* _HMC5983_ */
