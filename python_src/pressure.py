#!/usr/bin/python
#1;4601;0c
#I2C Bus Interface With Honeywell ABPDLNN100MG2A3
#Pressure Sensor

#import headers
import smbus 
import time # system time
import os # OS commands
import datetime as dt   # date and time
import subprocess
import os
import time

bus = smbus.SMBus(1)
address = 0x28

#********************************************************************************************************
#************************************** REGISTER VALUES *******************************************
#********************************************************************************************************

# MAX30100 I2C addresses
RADDRESS = 0x51 # 8bit address converted to 7bit + R
FIFO_DATA_REG = 0x00

#Diagnostic Status Bits
NORMAL_OPERATION = 0x00
COMMAND_MODE = 0x01
NORMAL_OPERATION = 0x02
DIAG_CONDITION = 0x03

#********************************************************************************************************
#************************************** CONSTANT VALUES *******************************************
#********************************************************************************************************

COUNTSMIN=int(0x0666) #Output Min Counts
COUNTSMAX=int(0x3999) #Output Max Counts
PMAX=100 # Pressure in mBar
PMIN=0


#********************************************************************************************************
#************************************** Function Definitions *******************************************
#********************************************************************************************************


#******************************************************
#Function mBar2mmhg()
#Converts from millibar pressure to inches of mercury 
#******************************************************
def mBar2mmhg(p_mbar):
	return (p_mbar * 0.7500);

#******************************************************
#Function readSample()
#Reads from 
#******************************************************
def readSample():
	dataSample= bus.read_i2c_block_data(address, FIFO_DATA_REG,2)
	myPres = (dataSample[0]<<8 )  | (dataSample[1] ) 
	return myPres;
#******************************************************
#Function counts2mbar()
#converts ADC Counts to pressure in mBar
#******************************************************
def counts2mBar(counts):
	pressure=(((counts-COUNTSMIN)*(PMAX-PMIN))/(COUNTSMAX-COUNTSMIN))+PMIN
	return float(pressure);
	
#********************************************************************************************************
#**************************************   TEST CODE   *************************************************
#********************************************************************************************************
print "the unit is on bus 0, Address"
print hex(address)

for i in range (0, 240 ):
	data=readSample()
	time.sleep(.1)
	pressure_m = counts2mBar(data)
	pressure_hg = mBar2mmhg(pressure_m)
	os.system("clear")
	print "pressure in mmhg is:" + str(pressure_hg)	+" pressure in mbar is:" + str(pressure_m)	
	time.sleep(.05)




