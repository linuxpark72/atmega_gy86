/*
 *  mpu6050.h
 *
 * Desc    : header for mpu6050
 * Created : 2019. 01. 08. (화) 10:39:12 KST
 * Author  : jeho park<linuxpark@gmail.com>
 */
#ifndef _MPU6050_
#define _MPU6050_


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

#define MPU6050_RA_ACCEL_XOUT_H     0x3B
#define MPU6050_RA_ACCEL_XOUT_L     0x3C
#define MPU6050_RA_ACCEL_YOUT_H     0x3D
#define MPU6050_RA_ACCEL_YOUT_L     0x3E 
#define MPU6050_RA_ACCEL_ZOUT_H     0x3F 
#define MPU6050_RA_ACCEL_ZOUT_L     0x40 
#define MPU6050_RA_TEMP_OUT_H       0x41
#define MPU6050_RA_TEMP_OUT_L       0x42
#define MPU6050_RA_GYRO_XOUT_H      0x43
#define MPU6050_RA_GYRO_XOUT_L      0x44
#define MPU6050_RA_GYRO_YOUT_H      0x45
#define MPU6050_RA_GYRO_YOUT_L      0x46
#define MPU6050_RA_GYRO_ZOUT_H      0x47
#define MPU6050_RA_GYRO_ZOUT_L      0x48

#define MPU6050_DLPF_BW_42          0x03

#define M_PI_F 3.1415926
#define CONSTANTS_ONE_G                         9.80665f        /* m/s^2        */
#define CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C     1.225f          /* kg/m^3       */
#define CONSTANTS_AIR_GAS_CONST                 287.1f          /* J/(kg * K)   */
#define CONSTANTS_ABSOLUTE_NULL_CELSIUS         -273.15f        /* 癈           */
#define CONSTANTS_RADIUS_OF_EARTH               6371000         /* meters (m)   */

//constant g // tobe fixed to 8g. but IMU need to correct at the same time
#define SENSOR_MAX_G 8.0f
#define SENSOR_MAX_W 2000.0f    //deg/s
#define ACC_SCALE  (SENSOR_MAX_G/32768.0f)
#define GYRO_SCALE  (SENSOR_MAX_W/32768.0f)

/* from avr_lib_mpu6050 */
#define MPU6050_CALIBRATEDACCGYRO 1 //set to 1 if is calibrated
#if MPU6050_CALIBRATEDACCGYRO == 1
#define MPU6050_AXOFFSET 0
#define MPU6050_AYOFFSET 0
#define MPU6050_AZOFFSET 0
#define MPU6050_AXGAIN 16384.0
#define MPU6050_AYGAIN 16384.0
#define MPU6050_AZGAIN 16384.0
#define MPU6050_GXOFFSET -42
#define MPU6050_GYOFFSET 9
#define MPU6050_GZOFFSET -29
#define MPU6050_GXGAIN 16.4
#define MPU6050_GYGAIN 16.4
#define MPU6050_GZGAIN 16.4
#endif

#define mpu6050_mahonysampleFreq 61.0f // sample frequency in Hz
#define mpu6050_mahonytwoKpDef (2.0f * 0.5f) // 2 * proportional gain
#define mpu6050_mahonytwoKiDef (2.0f * 0.1f) // 2 * integral gain

/* end from avr_lib_mpu6050 */

extern int mpu6050_test();
#endif /* _MPU6050_ */
