# atmega_gy86

  this project is experimental for testing the gy86, 10 axis sensor.


## Test environment

  - [atmega128 + gy86, 10 axis gyro sensor]

  - development host: 
      mac os x (avr-gcc, avrdude), 
	  linux ubuntu 16.04
      isp(AD-USBISP v06.0, $25).  
	  
	  please refer to docs/HOWTO.txt and Makefile.tmpl of this project.


  - target device: 
	
	  atmega 128A, stm32!



## current status of this project

  - ms5611   : 100%, 1'st, 2'st compensation and the result looks like below


		  C0[108], C1[44812], C2[46116], C3[28298], C4[24839], C5[30901], C6[28039], C7[35106] D1(9048194), D2(7950376), CRC(2), dT(39720.00)
		  1'st compensation:   Temperature(21.3276), Press(1014.5293 mbar)
		  2'st compensation:   Temperature(21.3276), Press(1014.5293 mbar)

		  C0[108], C1[44812], C2[46116], C3[28298], C4[24839], C5[30901], C6[28039], C7[35106] D1(9048166), D2(7950412), CRC(2), dT(39756.00)
		  1'st compensation:   Temperature(21.3288), Press(1014.5264 mbar)
		  2'st compensation:   Temperature(21.3288), Press(1014.5264 mbar)

		  C0[108], C1[44812], C2[46116], C3[28298], C4[24839], C5[30901], C6[28039], C7[35106] D1(9048156), D2(7950460), CRC(2), dT(39804.00)
		  1'st compensation:   Temperature(21.3305), Press(1014.5284 mbar)
		  2'st compensation:   Temperature(21.3305), Press(1014.5284 mbar)


  - HMC5883L : 40%

  - MPU6050  : 70%


## Todo

  - HMC5883L : delayed

  - ms5611   : completed

  - MPU6050  : working now 70%

  - drone    : maybe from April of 2019.


## Current Drone status

   - https://www.youtube.com/watch?v=d3Gm2NUxP8I

--- 
 welcome any feedback :)

 updated  Jan.  6. 2018
 JeHo Park <linuxpark@gmail.com> 
