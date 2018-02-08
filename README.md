# atmega_gy86

 this project is experimental project just for testing the gy86, 10 axis sensor.
at the later, i will apply this gy86 sensor and its code to my drone.


## Test environment

  - [atmega128 + gy86, 10 axis gyro sensor]

  - development host: mac os x (avr-gcc, avrdude), isp(AD-USBISP v06.0, $25).  please refer to docs/HOWTO.txt and Makefile.tmpl of this project.

  - target device: atmega 128A

  - if you want to know more about the environment, please sent me email.


## current status of this project

  - ms5611   : (90%, needed more valification), https://youtu.be/iHecBikMY48, 
               1. validation of crc, 2. check accuracy of compensated Press, Temp

  - HMC5883L : 40%

  - MPU6050  : 0%


## Todo

  - HMC5883L :
               1. analyze this sensor with its datasheet as soon as possible. 

  - ms5611   : 
               2. validation of crc -> check accuracy of compensated Press, Temp.

  - MPU6050  :
               3. maybe from mach of 2018.

  - drone    :
               4. replace the previous DMP code with the code of this project.


## Current Drone status

   - https://www.youtube.com/watch?v=d3Gm2NUxP8I

--- 
 welcome any feedback :)

 updated  Jan.  6. 2018
 JeHo Park <linuxpark@gmail.com> 
