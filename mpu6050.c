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
#include "mpu6050.h"

#define MPU6050_RA_CONFIG           0x1A
#define MPU6050_RA_GYRO_CONFIG      0x1B
#define MPU6050_GCONFIG_FS_SEL_BIT      4
#define MPU6050_GCONFIG_FS_SEL_LENGTH   2
#define MPU6050_GYRO_FS_250         0x00
#define MPU6050_GYRO_FS_500         0x01
#define MPU6050_GYRO_FS_1000        0x02
#define MPU6050_GYRO_FS_2000        0x03

#define MPU6050_RA_ACCEL_CONFIG     0x1C
#define MPU6050_RA_PWR_MGMT_1       0x6B
#define MPU6050_RA_SMPLRT_DIV       0x19
#define MPU6050_RA_INT_PIN_CFG      0x37
#define MPU6050_RA_WHO_AM_I         0x75
#define MPU6050_WHO_AM_I_BIT        6
#define MPU6050_WHO_AM_I_LENGTH     6

#define MPU6050_DLPF_BW_42          0x03

static uint8_t buffer[14];

/* stolen from crazepony-firmware-none, i2cdevlib */
static void MPU6050_initialize() {

	//PWR_MGMT_1    -- DEVICE_RESET 1
	//IICwriteByte(devAddr, MPU6050_RA_PWR_MGMT_1, 0x80);
	i2c_writeByte(0x68, MPU6050_RA_PWR_MGMT_1, 0x80);

	//delay_ms(50);
	_delay_ms(50);

	//SMPLRT_DIV    -- SMPLRT_DIV = 0  Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
	//IICwriteByte(devAddr, MPU6050_RA_SMPLRT_DIV, 0x00);
	i2c_writeByte(0x68, MPU6050_RA_SMPLRT_DIV, 0x00);

    //PWR_MGMT_1    -- SLEEP 0; CYCLE 0; TEMP_DIS 0; CLKSEL 3 (PLL with Z Gyro reference)
	//IICwriteByte(devAddr, MPU6050_RA_PWR_MGMT_1, 0x03);
	i2c_writeByte(0x68, MPU6050_RA_PWR_MGMT_1, 0x03);

	// INT_PIN_CFG
	//        (7)             (6)            (5)           (4)
	// -- INT_LEVEL_HIGH, INT_OPEN_DIS, LATCH_INT_DIS, INT_RD_CLEAR_DIS, 
	//        (3)                  (2)               (1)         (0)
	//    FSYNC_INT_LEVEL_HIGH, FSYNC_INT_DIS, I2C_BYPASS_EN, CLOCK_DIS
	//
	//IICwriteByte(devAddr, MPU6050_RA_INT_PIN_CFG, 0 << 7 | 0 << 6 | 0 << 5 | 0 << 4 | 0 << 3 | 0 << 2 | 1 << 1 | 0 << 0);
	i2c_writeByte(0x68, MPU6050_RA_INT_PIN_CFG, 0 << 7 | 0 << 6 | 0 << 5 | 0 << 4 | 0 << 3 | 0 << 2 | 1 << 1 | 0 << 0);

	//CONFIG        
	//-- EXT_SYNC_SET 0 (disable input pin for data sync) ; 
	//default DLPF_CFG = 0 => ACC bandwidth = 260Hz  GYRO bandwidth = 256Hz)
	//IICwriteByte(devAddr, MPU6050_RA_CONFIG, MPU6050_DLPF_BW_42);
	// 
	// Digital Low Pass Filter(DLPF)
	//
	//                       ACCEL (Fs 1kHz)     Gyro (Fs 1kHz)
	//                       bw    delay         bw    delay
	//MPU6050_DLPF_BW_42(3): 44Hz  4.9ms         42Hz  4.8ms 
	i2c_writeByte(0x68, MPU6050_RA_CONFIG, MPU6050_DLPF_BW_42);

	//MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
	//void MPU6050_setFullScaleGyroRange(uint8_t range) {
	//    IICwriteBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
	//}
	i2c_writeBits(0x68, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, MPU6050_GYRO_FS_2000);
	
	// Accel scale 8g (4096 LSB/g)
	//IICwriteByte(devAddr, MPU6050_RA_ACCEL_CONFIG, 2 << 3);
	i2c_writeByte(0x68, MPU6050_RA_ACCEL_CONFIG, 2 << 3);
}

uint8_t MPU6050_getDeviceID() {
	
	//I2Cdev_readBits(mpu6050.devAddr, MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, mpu6050.buffer);
	i2c_readBits(0x68, MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, buffer); 

	return buffer[0];
}

uint8_t MPU6050_testConnection() {
	return MPU6050_getDeviceID() == 0x34;
}

int mpu6050_test() {
#ifdef MPU6050_TEST
	printf("\r\n mpu6050\r\n");

	/* init */
	MPU6050_initialize();
	printf("mpu6050: %s\r\n", MPU6050_testConnection() ? "connected":"disconnected");


#endif /* MPU6050_TEST */
	return 0;
}
