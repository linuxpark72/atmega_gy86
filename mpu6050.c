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

#define MPU6050_ADDR (0x68 <<1) 

volatile uint8_t buffer[14];
/* from avr_lib_mpu6050 */
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
volatile float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;
/*
 * Mahony update function (for 6DOF)
 */
void mpu6050_mahonyUpdate(float gx, float gy, float gz, float ax, float ay, float az) {
	float norm;
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		norm = sqrt(ax * ax + ay * ay + az * az);
		ax /= norm;
		ay /= norm;
		az /= norm;

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		halfvx = q1 * q3 - q0 * q2;
		halfvy = q0 * q1 + q2 * q3;
		halfvz = q0 * q0 - 0.5f + q3 * q3;

		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		// Compute and apply integral feedback if enabled
		if(mpu6050_mahonytwoKiDef > 0.0f) {
			integralFBx += mpu6050_mahonytwoKiDef * halfex * (1.0f / mpu6050_mahonysampleFreq);	// integral error scaled by Ki
			integralFBy += mpu6050_mahonytwoKiDef * halfey * (1.0f / mpu6050_mahonysampleFreq);
			integralFBz += mpu6050_mahonytwoKiDef * halfez * (1.0f / mpu6050_mahonysampleFreq);
			gx += integralFBx;	// apply integral feedback
			gy += integralFBy;
			gz += integralFBz;
		} else {
			integralFBx = 0.0f;	// prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gx += mpu6050_mahonytwoKpDef * halfex;
		gy += mpu6050_mahonytwoKpDef * halfey;
		gz += mpu6050_mahonytwoKpDef * halfez;
	}

	// Integrate rate of change of quaternion
	gx *= (0.5f * (1.0f / mpu6050_mahonysampleFreq));		// pre-multiply common factors
	gy *= (0.5f * (1.0f / mpu6050_mahonysampleFreq));
	gz *= (0.5f * (1.0f / mpu6050_mahonysampleFreq));
	qa = q0;
	qb = q1;
	qc = q2;
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx);

	// Normalise quaternion
	norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 /= norm;
	q1 /= norm;
	q2 /= norm;
	q3 /= norm;
}

/*
 * update quaternion
 */
void mpu6050_updateQuaternion() {
	int16_t ax = 0;
	int16_t ay = 0;
	int16_t az = 0;
	int16_t gx = 0;
	int16_t gy = 0;
	int16_t gz = 0;
	double axg = 0;
	double ayg = 0;
	double azg = 0;
	double gxrs = 0;
	double gyrs = 0;
	double gzrs = 0;

	i2c_readBytes(MPU6050_ADDR, MPU6050_RA_ACCEL_XOUT_H, 14, buffer);
	ax = (int16_t)((buffer[0] << 8) | buffer[1]);
	ay = (int16_t)((buffer[2] << 8) | buffer[3]);
	az = (int16_t)((buffer[4] << 8) | buffer[5]);
	gx = (int16_t)((buffer[8] << 8) | buffer[9]);
	gy = (int16_t)((buffer[10] << 8) | buffer[11]);
	gz = (int16_t)((buffer[12] << 8) | buffer[13]);


	#if MPU6050_CALIBRATEDACCGYRO == 1
	axg = (double)(ax-MPU6050_AXOFFSET)/MPU6050_AXGAIN;
	ayg = (double)(ay-MPU6050_AYOFFSET)/MPU6050_AYGAIN;
	azg = (double)(az-MPU6050_AZOFFSET)/MPU6050_AZGAIN;
	gxrs = (double)(gx-MPU6050_GXOFFSET)/MPU6050_GXGAIN*0.01745329; //degree to radians
	gyrs = (double)(gy-MPU6050_GYOFFSET)/MPU6050_GYGAIN*0.01745329; //degree to radians
	gzrs = (double)(gz-MPU6050_GZOFFSET)/MPU6050_GZGAIN*0.01745329; //degree to radians
	#else
	axg = (double)(ax)/MPU6050_AGAIN;
	ayg = (double)(ay)/MPU6050_AGAIN;
	azg = (double)(az)/MPU6050_AGAIN;
	gxrs = (double)(gx)/MPU6050_GGAIN*0.01745329; //degree to radians
	gyrs = (double)(gy)/MPU6050_GGAIN*0.01745329; //degree to radians
	gzrs = (double)(gz)/MPU6050_GGAIN*0.01745329; //degree to radians
	#endif

    //compute data
    mpu6050_mahonyUpdate(gxrs, gyrs, gzrs, axg, ayg, azg);
}

void mpu6050_getQuaternion(double *qw, double *qx, double *qy, double *qz) {
	*qw = q0;
	*qx = q1;
	*qy = q2;
	*qz = q3;
}

/*
 * get euler angles
 * aerospace sequence, to obtain sensor attitude:
 * 1. rotate around sensor Z plane by yaw
 * 2. rotate around sensor Y plane by pitch
 * 3. rotate around sensor X plane by roll
 */
void mpu6050_getRollPitchYaw(double *roll, double *pitch, double *yaw) {
	*yaw = atan2(2*q1*q2 - 2*q0*q3, 2*q0*q0 + 2*q1*q1 - 1);
	*pitch = -asin(2*q1*q3 + 2*q0*q2);
	*roll = atan2(2*q2*q3 - 2*q0*q1, 2*q0*q0 + 2*q3*q3 - 1);
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
				 MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, (uint8_t *)buffer); 
	return buffer[0];
}

uint8_t MPU6050_testConnection() {
	return MPU6050_getDeviceID() == 0x34;
}

int mpu6050_test() {
#ifdef MPU6050_TEST
	double qw = 1.0f;
	double qx = 0.0f;
	double qy = 0.0f;
	double qz = 0.0f;
	double roll = 0.0f;
	double pitch = 0.0f;
	double yaw = 0.0f;
#ifdef _PROFILE_FREG_
	tm_t    prev, current, tmp;
	float   delay, delay_sum = 0;
	int     lcnt = 0;

	DDRB |= 1; 
	init_timer0();
	timer0(&prev);
#endif

	if (!MPU6050_testConnection()) {
		return -1;
	}
	printf("\r\n\r\nmpu6050(0x%x) connected!\r\n", MPU6050_getDeviceID() << 1);

	/* init */
	MPU6050_initialize();

	/* load */
	while(1) {
		
		/* from avr_lib_mpu6050 */
		mpu6050_updateQuaternion();
		mpu6050_getQuaternion(&qw, &qx, &qy, &qz);
#if !defined(_MATLAB_)
		mpu6050_getRollPitchYaw(&roll, &pitch, &yaw);
#endif

#if defined(_MATLAB_)
		/**********************  Matlab aerospace  **************/
		printf("%4.3f\t%4.3f\t%4.3f\t%4.3f\t ", qw, qx, qy, qz);
#endif	
#ifdef _PROFILE_FREG_
		/**********************  Profiling **********************/
		PORTB = 0x0;
		timer0(&current);
		delay = timer0_cal(&current, &prev);
		delay_sum += delay;
		
		if (++lcnt == TIMER_LOOP_CNT) {
			/* avg: delay (ms) */
			float avg_hz = 1 / (delay_sum * TIMER_LOOP_HZ);

			/* TODO */
			printf("%4.3f(KHz) qw:%4.3f qx:%4.3f qy:%4.3f "
				   "qz:%4.3f Roll:%4.3f Pitch:%4.3f Yaw:%4.3f\r\n",
				   avg_hz, qw, qx, qy, qz, roll * DEGREE, pitch * DEGREE, yaw * DEGREE);
			/* TODO -end */
			delay_sum = 0;
            lcnt = 0;
		}
		timer0(&tmp);
		timer0_update(&tmp, &prev);
		PORTB = 0x1;
		/**********************  Profiling **********************/
#endif
	}

#endif /* MPU6050_TEST */
	return 0;
}
