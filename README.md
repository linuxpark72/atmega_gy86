# atmega_gy86
 this project is experimental project just for testing the gy86, 10 axis sensor.
at the later, i will apply this gy86 sensor and its code to my drone.


## Test environment

  - [atmega128 + gy86, 10 axis gyro sensor]
  - development host: mac os x (avr-gcc, avrdude), isp(AD-USBISP v06.0, $25).  please refer to docs/HOWTO.txt and Makefile.tmpl of this project.
  - target device: atmega 128A
  - if you want to know more about the environment, please sent me email.


## current status of this project

  - ms5611   : 90%,
               it was tested but the first two coefficients return negative integers ..
			   i think my sensor was broken so i will change the gy86 sensor with others later. 

  - HMC5883L : 0%
  - MPU6050  : 0%


## Todo

  - HMC5883L :
               1. analyze this sensor with its datasheet as soon as possible. 
  - ms5611   : 
               2. change current sensor with another and test again.
  - MPU6050  :
               3. maybe from febuary of 2018.
  - drone    :
               4. replace the previous DMP code with the code of this project.


## Current Drone status

   - https://www.youtube.com/watch?v=d3Gm2NUxP8I

--- 
 welcome any feedback :)

 Jan.  1. 2018
 JeHo Park <linuxpark@gmail.com> 
