# atmega_gy86

  this project is for experimental testing the gy86, which is 10 axis sensor.



## Test environment

  * atmega128 + gy86, 10 axis gyro sensor

  * development host: 
  
      mac os x (avr-gcc, avrdude), 
	  linux ubuntu 16.04
      isp(AD-USBISP v06.0, $25).  
	  
	  please refer to docs/HOWTO.txt and Makefile.tmpl of this project.


  * target device: 
	
	  atmega 128A, stm32!



## current status of this project


  - ms5611   : completed 
  
	      * 1'st, 2'st compensation and the result looks like below


		  C0[108], C1[44812], C2[46116], C3[28298], C4[24839], C5[30901], C6[28039], C7[35106] D1(9048194), D2(7950376), CRC(2), dT(39720.00)
		  1'st compensation:   Temperature(21.3276), Press(1014.5293 mbar)
		  2'st compensation:   Temperature(21.3276), Press(1014.5293 mbar)

		  C0[108], C1[44812], C2[46116], C3[28298], C4[24839], C5[30901], C6[28039], C7[35106] D1(9048166), D2(7950412), CRC(2), dT(39756.00)
		  1'st compensation:   Temperature(21.3288), Press(1014.5264 mbar)
		  2'st compensation:   Temperature(21.3288), Press(1014.5264 mbar)

		  C0[108], C1[44812], C2[46116], C3[28298], C4[24839], C5[30901], C6[28039], C7[35106] D1(9048156), D2(7950460), CRC(2), dT(39804.00)
		  1'st compensation:   Temperature(21.3305), Press(1014.5284 mbar)
		  2'st compensation:   Temperature(21.3305), Press(1014.5284 mbar)


  - MPU6050  : 70%

		  mpu6050(0x68) connected!
		  0.421(KHz) qw:0.998 qx:0.002 qy:-0.022 qz:-0.055 Roll:-0.001 Pitch:0.043 Yaw:0.111
		  0.429(KHz) qw:0.986 qx:0.005 qy:-0.022 qz:0.165 Roll:-0.017 Pitch:0.043 Yaw:-0.331
		  0.434(KHz) qw:0.926 qx:0.010 qy:-0.020 qz:0.378 Roll:-0.033 Pitch:0.030 Yaw:-0.776
		  0.441(KHz) qw:0.820 qx:0.014 qy:-0.018 qz:0.572 Roll:-0.043 Pitch:0.013 Yaw:-1.219
		  0.432(KHz) qw:0.674 qx:0.018 qy:-0.014 qz:0.738 Roll:-0.044 Pitch:-0.007 Yaw:-1.661
		  0.434(KHz) qw:0.495 qx:0.020 qy:-0.010 qz:0.868 Roll:-0.038 Pitch:-0.024 Yaw:-2.105
		  0.434(KHz) qw:0.292 qx:0.022 qy:-0.005 qz:0.956 Roll:-0.023 Pitch:-0.039 Yaw:-2.547
		  0.428(KHz) qw:0.075 qx:0.022 qy:-0.001 qz:0.997 Roll:-0.005 Pitch:-0.045 Yaw:-2.991
		  0.430(KHz) qw:-0.146 qx:0.022 qy:0.005 qz:0.989 Roll:0.015 Pitch:-0.042 Yaw:2.849
		  0.431(KHz) qw:-0.358 qx:0.020 qy:0.009 qz:0.933 Roll:0.031 Pitch:-0.032 Yaw:2.408
		  0.438(KHz) qw:-0.554 qx:0.018 qy:0.013 qz:0.832 Roll:0.043 Pitch:-0.015 Yaw:1.966
		  0.434(KHz) qw:-0.724 qx:0.014 qy:0.017 qz:0.690 Roll:0.045 Pitch:0.005 Yaw:1.523
		  0.436(KHz) qw:-0.858 qx:0.011 qy:0.020 qz:0.513 Roll:0.039 Pitch:0.023 Yaw:1.078
		  0.437(KHz) qw:-0.950 qx:0.006 qy:0.022 qz:0.313 Roll:0.025 Pitch:0.038 Yaw:0.637
		  0.433(KHz) qw:-0.995 qx:0.001 qy:0.023 qz:0.096 Roll:0.005 Pitch:0.045 Yaw:0.193
		  0.432(KHz) qw:-0.992 qx:-0.004 qy:0.022 qz:-0.124 Roll:-0.014 Pitch:0.043 Yaw:-0.249



  - HMC5883L : 40%



## Todo

  - HMC5883L : delayed

  - ms5611   : completed

  - MPU6050  : working now 70%

  - HMC5883L : delayed

  - drone    : maybe from April of 2019.



## previous my quadcopter

   - https://www.youtube.com/watch?v=d3Gm2NUxP8I


--- 
 welcome any feedback :)

 updated  Jan.  17. 2019
 JeHo Park <linuxpark@gmail.com> 
