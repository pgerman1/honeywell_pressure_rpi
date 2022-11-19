# Honeywell Pressure Transducer Driver

## Overview
This Repository Contains a pressure transducer driver for the Honeywell ABPDLNN100MG2A3 Pressure Sensor communicating over an I2C Bus on the raspberry pi.  Driver is setup to initialize the I2C Bus and read data from a working 

Similar I2C Pressure Sensor models could be used with this driver with minor modifications to the Member functions of this class.

This driver was designed for the Raspberry pi using the wiringPi and wiringPiI2C Library

## Usage

### Constructors

Four constructors are provided depending on user intent.

Pressure();		  //base constructor
Pressure(int);		  //Constructor, Setting Bus
Pressure(int,int);	  //Constructor, Setting Bus And Address
Pressure(int,int,int,int);	  //Constructor, Setting Bus And Address, Min/Max

### Methods

float mbar2mmhg(float)

float counts2mbar(int)   //convert ADC Counts to mBar
float counts2mmhg(int)   //convert ADC Counts to mmhg

void getFd();             //get format descirptor
void getAddress();        //return I2C Address ** Fix
void getBus();        //return I2C Address ** Fix

int readCounts();         //Read Data in ADC Counts

float readPressure_mmhg();//Read Data in mmHg

float readPressure_mbar();//Read Data in mBar

Object Atributes
