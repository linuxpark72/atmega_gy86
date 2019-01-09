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

static uint8_t buffer[7];
static int16_t accADC[3];
static int16_t gyroADC[3];
static float   accRaw[3];      //m/s^2
static float   gyroRaw[3];     //rad/s

static void MPU6050AccRead(int16_t *accData) {
	uint8_t buf[6];

	i2c_readBytes(0x68, MPU6050_RA_ACCEL_XOUT_H, 6, buf);
	accData[0] = (int16_t)((buf[0] << 8) | buf[1]);
	accData[1] = (int16_t)((buf[2] << 8) | buf[3]);
	accData[2] = (int16_t)((buf[4] << 8) | buf[5]);
}

static void MPU6050GyroRead(int16_t *gyroData) {
	uint8_t buf[6];

	i2c_readBytes(0x68, MPU6050_RA_GYRO_XOUT_H, 6, buf);
	gyroData[0] = (int16_t)((buf[0] << 8) | buf[1]) ;
	gyroData[1] = (int16_t)((buf[2] << 8) | buf[3]) ;
	gyroData[2] = (int16_t)((buf[4] << 8) | buf[5]) ;
}

/* stolen from crazepony-firmware-none, i2cdevlib */
static void MPU6050_initialize() {

	//PWR_MGMT_1    -- DEVICE_RESET 1
	i2c_writeByte(0x68, MPU6050_RA_PWR_MGMT_1, 0x80);

	//delay_ms(50);
	_delay_ms(50);

	//SMPLRT_DIV    -- SMPLRT_DIV = 0  Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
	i2c_writeByte(0x68, MPU6050_RA_SMPLRT_DIV, 0x00);

    //PWR_MGMT_1    -- SLEEP 0; CYCLE 0; TEMP_DIS 0; CLKSEL 3 (PLL with Z Gyro reference)
	i2c_writeByte(0x68, MPU6050_RA_PWR_MGMT_1, 0x03);

	// INT_PIN_CFG
	//        (7)             (6)            (5)           (4)
	// -- INT_LEVEL_HIGH, INT_OPEN_DIS, LATCH_INT_DIS, INT_RD_CLEAR_DIS, 
	//        (3)                  (2)               (1)         (0)
	//    FSYNC_INT_LEVEL_HIGH, FSYNC_INT_DIS, I2C_BYPASS_EN, CLOCK_DIS
	i2c_writeByte(0x68, MPU6050_RA_INT_PIN_CFG, 0 << 7 | 0 << 6 | 0 << 5 | 0 << 4 | 0 << 3 | 0 << 2 | 1 << 1 | 0 << 0);

	//CONFIG        
	//-- EXT_SYNC_SET 0 (disable input pin for data sync) ; 
	//default DLPF_CFG = 0 => ACC bandwidth = 260Hz  GYRO bandwidth = 256Hz)
	//IICwriteByte(devAddr, MPU6050_RA_CONFIG, MPU6050_DLPF_BW_42);
	// Digital Low Pass Filter(DLPF)
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
	i2c_readBits(0x68, MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, buffer); 
	return buffer[0];
}

uint8_t MPU6050_testConnection() {
	return MPU6050_getDeviceID() == 0x34;
}

int mpu6050_test() {
#ifdef MPU6050_TEST
	int i;

	if (!MPU6050_testConnection()) {
		return -1;
	}
	printf("\r\nmpu6050(0x%x) connected!\r\n", MPU6050_getDeviceID() << 1);
	_delay_ms(2000);

	/* init */
	MPU6050_initialize();
	
	/* load */
	while(1) {
		MPU6050AccRead(accADC);
		MPU6050GyroRead(gyroADC);

		for (i=0; i<3; i++) {
			accRaw[i]= (float)accADC[i] *ACC_SCALE * CONSTANTS_ONE_G ;
			gyroRaw[i]=(float)gyroADC[i] * GYRO_SCALE * M_PI_F /180.f;      //deg/s
		}

		printf("ACCEL[x:%4.3f, y:%4.3f, z:%4.3f], GYRO[x:%4.3f, y:%4.3f z:%4.3f]\r\n",
			   accRaw[0], accRaw[1], accRaw[2], gyroRaw[0], gyroRaw[1], gyroRaw[2]);
		_delay_ms(10);
	}

#endif /* MPU6050_TEST */
	return 0;
}
