#!/usr/bin/gnuplot

#Plotting torque and power curves, using GNU plot
#USAGE:
#1 Copy your engine settings to the section below
#2 Execute this file, or load it in GNU plot

#---------------------------
#Torque in Nm of a non-rotating engine (@ 0 RPM)
zerotorque = 600

#Max torque point. Torque in Nm, engine speed in rad/s
maxtorque = 640
w_maxtorque = 576

#Max power point. Power in W, engine speed in rad/s
maxpower = 440000
w_maxpower = 764

#Engine speed in rad/s where the engine doesn't generate torque anymore
w_zero = 850
#---------------------------

M0=zerotorque;M1=maxtorque;w1=w_maxtorque;p2=maxpower;w2=w_maxpower;w3=w_zero

Mp1(w)=M1-(M1-M0)*((w-w1)/w1)**2

dw2=w2-w1
A2=(3+dw2/w2)*p2/w2-3*M1
B2=(2+dw2/w2)*p2/w2-2*M1
Mp2(w)=M1+A2*((w-w1)/(w2-w1))**2-B2*((w-w1)/(w2-w1))**3

dw3=w3-w2
Mp3(w)=p2/w*(1-((w-w2)/(w3-w2))**2)

Mp(w) = (w<w1)? Mp1(w) : (w<w2) ? Mp2(w) :Mp3(w)
P(w)=Mp(w)*w/1000
plot [w=0:w3] Mp(w),P(w)
pause -1
