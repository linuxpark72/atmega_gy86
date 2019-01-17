%   SIMPLE CODE FOR VISUALIZING QUATERNION DATA FROM IMU (Ex.MPU6050 DMP)
%          You have to send data According to:
%          qw(\t)qx(\t)qy(\t)qz(\t)(' ')

clear; clc; close all; 
%Creat Port
s = serial('/dev/ttyUSB0', 'BaudRate', 9600)
%Open Port 
fopen(s);
%Set Terminator
set(s,'Terminator',' ');fscanf(s);
%Read First Data for Calibration 
a(1,:)=fscanf(s,'%f');
[yaw_c, pitch_c, roll_c]=quat2angle(a(1,:));
for frame=1:1:1000
    frame
    %Read Data;
    a(frame,:)=fscanf(s,'%f');
    %Convert Quaternion to Roll Pitch Yaw
    [yaw, pitch, roll] = quat2angle(a(frame,:));
    %Visualize Data On Sphere Or any Other Objects
    [x,y,z] = sphere;h = surf(x,y,z);axis('square'); 
    title('AOL TEAM')
    xlabel('x'); ylabel('y'); zlabel('z');
    %Rotate Object
    rotate(h,[1,0,0],(roll-roll_c)*180/pi)
    rotate(h,[0,1,0],(pitch-pitch_c)*180/pi)
    rotate(h,[0,0,1],(yaw-yaw_c)*180/pi+90)
    view(0,0);
    drawnow
end
fclose(s);