#!/usr/bin/python3
"""
This Class is a pressure transducer driver for the
honeywell ABPDLNN100MG2A3 Pressure Sensor communicating
over an I2C Bus.

Similar I2C Pressure Sensor models could be used
with this driver with minor modifcations to the
Member functions of this class

This driver was designed for the Raspberry pi using the
smbus library for python
"""

#Import Modules
import smbus


#********************************************************************************
#***************************** CONSTANT VALUES **********************************
#********************************************************************************

DEFAULT_COUNTSMIN = int(0x0666) 	# Output Min Counts
DEFAULT_COUNTSMAX = int(0x3999) 	# Output Max Counts
DEFAULT_BUS = 1						# Default I2C Bus
DEFAULT_PMAX = 100 		      		# Pressure in mBar
DEFAULT_PMIN = 0				  	# Min Pressure in mBar
DEFAULT_ADDRESS = 0x28				# Default I2C Address
MMHG_MBAR_RATIO = 0.7500			# Conversion Rate from mBar to mmHg - Pro Tip - no Magic Numbers.

# Class Definition
class Pressure:

	#Base Constructor Method, Default Settings

	def __init__(self) -> None:
		# Set Default Values
		self.address = DEFAULT_ADDRESS
		self.minCounts = DEFAULT_COUNTSMIN
		self.maxCounts = DEFAULT_COUNTSMAX
		self.pressureMin = DEFAULT_PMIN
		self.pressureMax = DEFAULT_PMAX
		
		# Setup Default Bus Object
		self.bus = smbus.SMBus(DEFAULT_BUS)

	#******************************************************
	# Function mBar2mmhg()
	# Converts from millibar pressure to inches of mercury
	# args - pMbar 
	#******************************************************
	def mBar2mmhg(pMbar):
		return (pMbar * MMHG_MBAR_RATIO)

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
