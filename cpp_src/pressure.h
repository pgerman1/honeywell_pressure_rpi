/**************************************************
* Pressure.h
* This Class is a pressure transducer driver for the
* honeywell ABPDLNN100MG2A3 Pressure Sensor communicating
* over an I2C Bus.
*
* Similar I2C Pressure Sensor models could be used
* with this driver with minor modifcations to the
* Member functions of this class
*
* This driver was designed for the Raspberry pi using
* the wiringPi Library
*
***************************************************/

#ifndef PRESSURE_H
#define PRESSURE_H

//Include Headers
#include<stdlib.h>
#include<time.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
//#include <curses.h>
//Definitions
#define DEFAULT_ADDRESS 0x28
class Pressure {

      //Define Device Status Register Value
	static const int STATUS_REG = 0x00;

      //Define Device Status Register Values
	static const int NORMAL_OPERATION = 0x00;
	static const int COMMAND_MODE = 0x01;
	static const int STALE_DATA = 0x02;
	static const int DIAG_CONDITION = 0x03;

      //Define Device Data Register Value
	static const int DATA_REG = 0x00;

	  //Define Device min and max ADC counts
	static const int ADC_MIN_COUNTS = 0x0666;
	static const int ADC_MAX_COUNTS = 0x3999;

    public:
  	// Public Member Functions

	//Constructors
	Pressure();		  //base constructor
	Pressure(int);		  //Constructor, Setting Bus
	Pressure(int,int);	  //Constructor, Setting Bus And Address
	Pressure(int,int,int,int);	  //Constructor, Setting Bus And Address, 
									//Min/Max Pressure Range
	
	//Getter Functions
	int getAddress();         //return I2C Address 
	int getBus();        	  //return I2C Bus Number 
	int getCountsMin();		  //Return min ADC counts
	int getCountsMax();       //Return max ADC counts
	int getPressureMin();	  //Return min Pressure Range (mBar)
	int getPressureMax();     //Return max Pressure Range (mBar)
	int getFd();			  //Return Format Descriptor

	//IO Reader Functions
	int readCounts();         //Read Data in ADC Counts
	float readPressure_mmhg();//Read Data in mmHg
	float readPressure_mbar();//Read Data in mBar

	//Conversion Functions
	float mbar2mmhg(float);   //convert mbar to mmhg
	float counts2mbar(int);   //convert ADC Counts to mBar
	float counts2mmhg(int);   //convert ADC Counts to mmhg
	
	//Private Data
    private:
	
	//Atributes
  	int countsmin; // Max ADC Counts
  	int countsmax; // Min ADC Counts
  	int pmin;      // Min Pressure (mBar)
  	int pmax;      // Max Pressure (mBar)
  	int address;   // I2C address of the Transducer IC
  	int bus;       // I2C Bus of the Transducer
  	int fd;	       // Transducer Format Descriptor

  // Private Member Functions - Setters for Setup
	void setCounts(int,int);  //set min and max counts
	void setSpan(int,int);	  //set min and max pressure
	void setAddress(int);     //set address
	void setBus(int);         //set I2C Bus
	void setFd();          //set format descirptor

};

#include "pressure.cpp"

#endif
