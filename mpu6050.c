/*
 *  mpu6050.c
 *
 * Desc    : mpu6050 gyro sensor
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
#include "timer.h"
#include "mpu6050.h"

static uint8_t buffer[7];
int16_t accADC[3] = {0};
int16_t gyroADC[3] = {0};
#define MPU6050_ADDR (0x68 <<1) 

static uint8_t MPU6050AccRead() {
	uint8_t buf[6];
	uint8_t c;

	c = i2c_readBytes(MPU6050_ADDR, MPU6050_RA_ACCEL_XOUT_H, 6, buf);
	accADC[0] = (int16_t)((buf[0] << 8) | buf[1]);
	accADC[1] = (int16_t)((buf[2] << 8) | buf[3]);
	accADC[2] = (int16_t)((buf[4] << 8) | buf[5]);

	return c;
}

static uint8_t MPU6050GyroRead() {
	uint8_t buf[6];
	uint8_t c;

	c = i2c_readBytes(MPU6050_ADDR, MPU6050_RA_GYRO_XOUT_H, 6, buf);
	gyroADC[0] = (int16_t)((buf[0] << 8) | buf[1]) ;
	gyroADC[1] = (int16_t)((buf[2] << 8) | buf[3]) ;
	gyroADC[2] = (int16_t)((buf[4] << 8) | buf[5]) ;

	return c;
}

/* stolen from crazepony-firmware-none, i2cdevlib */
static void MPU6050_initialize() {

	/* 
	 * 1. Reset
	 * PWR_MGMT_1    -- DEVICE_RESET 1 
	 */
	i2c_writeByte(MPU6050_ADDR, MPU6050_RA_PWR_MGMT_1, 0x80);
	_delay_ms(50);


	/*
	 * 2. Sampling rate
	 * Sample Rate = Gyroscope Output Rate(8 or 1Khz) / (1 + SMPLRT_DIV)
	 * 8khz when DLPF off, 1khz when DLPF on
	 */
	i2c_writeByte(MPU6050_ADDR, MPU6050_RA_SMPLRT_DIV, 0x00);

    /*
	 * 3. Sleep & Clock
	 * PWR_MGMT_1    
	 * SLEEP 0; CYCLE 0; TEMP_DIS 0; CLKSEL 3 (PLL with Z Gyro reference)
	 */
	i2c_writeByte(MPU6050_ADDR, MPU6050_RA_PWR_MGMT_1, 0x03);

	/* 
	 * 4. INT_PIN_CFG
	 *       (7)             (6)            (5)           (4)
	 * -- INT_LEVEL_HIGH, INT_OPEN_DIS, LATCH_INT_DIS, INT_RD_CLEAR_DIS, 
	 *       (3)                  (2)               (1)         (0)
	 *   FSYNC_INT_LEVEL_HIGH, FSYNC_INT_DIS, I2C_BYPASS_EN, CLOCK_DIS
     */
	i2c_writeByte(MPU6050_ADDR, MPU6050_RA_INT_PIN_CFG, 
	  0 << 7 | 0 << 6 | 0 << 5 | 0 << 4 | 0 << 3 | 0 << 2 | 1 << 1 | 0 << 0);

	/* 
	 * 5. Digital Low Pass Filter(DLPF)
	 * -- EXT_SYNC_SET 0 (disable input pin for data sync) ; 
	 */
	i2c_writeByte(MPU6050_ADDR, MPU6050_RA_CONFIG, MPU6050_DLPF_BW_42);

	/* 
	 * 6. Gyro Scaling
	 */
	i2c_writeBits(MPU6050_ADDR, MPU6050_RA_GYRO_CONFIG, 
	  MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, MPU6050_GYRO_FS_2000);
	
	/*
	 * 7. Accel scale 8g (4096 LSB/g)
	 */
	i2c_writeByte(MPU6050_ADDR, MPU6050_RA_ACCEL_CONFIG, 2 << 3);
}

uint8_t MPU6050_getDeviceID() {
	i2c_readBits(MPU6050_ADDR, MPU6050_RA_WHO_AM_I,
				 MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, buffer); 
	return buffer[0];
}

uint8_t MPU6050_testConnection() {
	return MPU6050_getDeviceID() == 0x34;
}

int mpu6050_test() {
#ifdef MPU6050_TEST
	int i;
	float   accRaw[3];      //m/s^2
	float   gyroRaw[3];     //rad/s
	tm_t    prev, current;
	float   delay, delay_sum = 0;
	int     lcnt = 0;

	init_timer0();

	if (!MPU6050_testConnection()) {
		return -1;
	}
	printf("\r\n\r\nmpu6050(0x%x) connected!\r\n", MPU6050_getDeviceID() << 1);

	/* init */
	MPU6050_initialize();

	timer0(&prev);
	/* load */
	while(1) {
		MPU6050AccRead();
		MPU6050GyroRead();

		for (i=0; i<3; i++) {
			accRaw[i]= (float)(accADC[i] * ACC_SCALE * CONSTANTS_ONE_G);
			gyroRaw[i]=(float)(gyroADC[i] * GYRO_SCALE * M_PI_F)/180.f;      //deg/s
		}

		timer0(&current);
		delay = timer0_cal(&current, &prev);
		delay_sum += delay;
		timer0_update(&current, &prev);
		
		
		if (++lcnt == TIMER_LOOP_CNT) {
			/* avg: delay (ms) */
			float avg_hz = 1 / (delay_sum * TIMER_LOOP_HZ);

			/* TODO */
#if 1
			printf("%4.2f(KHz) Accel(x:%4.3f, y:%4.3f, z:%4.3f) Gyro(x:%4.3f, y:%4.3f z:%4.3f)\r\n",
				   avg_hz, accRaw[0], accRaw[1], accRaw[2], gyroRaw[0], gyroRaw[1], gyroRaw[2]);
#else

			printf("%4.2f(KHz)\r\n", avg_hz);
#endif


			/* TODO -end */
			delay_sum = 0;
            lcnt = 0;
		}
	}

#endif /* MPU6050_TEST */
	return 0;
}
