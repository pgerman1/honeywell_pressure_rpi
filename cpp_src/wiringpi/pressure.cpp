#include "pressure.h"

using namespace std;

/**************************************************
* Constructor Method
Creates a Default Sensor
-----------------------------------
Address = 0x28
Bus = /dev/i2c-1
Pressure Span = 0-100 mBar
Min ADC counts = ADC_MIN_COUNTS
Max ADC counts = ADC_MAX_COUNTS
-----------------------------------
Args: I2C Bus Number
Returns: New Pressure Sensor Object
***************************************************/

Pressure::Pressure(){
	setBus(1);
	setAddress(DEFAULT_ADDRESS);
	setSpan(0,100);
	setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);
	setFd();
}

/**************************************************
* Constructor Method
Creates a Default Sensor, I2C Bus Declared
-----------------------------------
Address = 0x28
Bus = user defined
Pressure Span = 0-100 mBar
Min ADC counts = ADC_MIN_COUNTS
Max ADC counts = ADC_MAX_COUNTS
-----------------------------------
Args: I2C Bus Number
Returns: New Pressure Sensor Object
***************************************************/

Pressure::Pressure(int myBus){
	setBus(myBus);
	setAddress(DEFAULT_ADDRESS);
	setSpan(0,100);
	setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);
	setFd();
}

/**************************************************
* Constructor Method
Creates a Default Sensor, I2C Bus Declared, user-defined address
-----------------------------------
Pressure Span = 0-100 mBar
Min ADC counts = ADC_MIN_COUNTS
Max ADC counts = ADC_MAX_COUNTS
-----------------------------------
Args: I2C Bus Number
Returns: New Pressure Sensor Object
***************************************************/

Pressure::Pressure(int myBus, int myAddress){
	setBus(myBus);
	setAddress(myAddress);
	setSpan(0,100);
	setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);
	setFd();
}
/**************************************************
* Constructor Method
Creates a Default Sensor, I2C Bus Declared, user-defined address, User Defined Span)
-----------------------------------
Min ADC counts = ADC_MIN_COUNTS
Max ADC counts = ADC_MAX_COUNTS
-----------------------------------
Args: I2C Bus Number
Returns: New Pressure Sensor Object
***************************************************/
Pressure::Pressure(int myBus, int myAddress,int myMin, int myMax){
	setBus(myBus);
	setAddress(myAddress);
	setSpan(myMin,myMax);
	setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);
	setFd();
}


/**************************************************
* Function counts2mbar()
Descritpion: Converts Pressure Transducer ADC Counts
to pressure in millibars.
Args: counts - Transducer ADC Counts
Returns: pressure - Pressure in Milibars
***************************************************/
float Pressure::counts2mbar(int counts){

	float myPressure;

	myPressure=((((float)counts-countsmin)*(pmax-pmin))/((float)countsmax-(float)countsmin))+pmin;
	return myPressure;
}
/**************************************************
* Function counts2mmhg()
Descritpion: Converts Pressure Transducer ADC Counts
to pressure in mmhg.
Args: counts - Transducer ADC Counts
Returns: pressure - Pressure in mmHg
***************************************************/
float Pressure::counts2mmhg(int counts){

	return (counts2mbar(counts)*0.7500);
}

/**************************************************
* Function mbar2mmhg()
Descritpion: Converts Pressure Transducer ADC Counts
to pressure in millibars.
Args: p_mbar -
Returns: pressure in mmHg
***************************************************/
float Pressure::mbar2mmhg(float p_mbar){

	return p_mbar*0.7500;  // .75 factor conversion
}
/**************************************************
* Function setAddress()
Descritpion: Sets the I2C Address of the pressure sensor
Args: myAddress - I2C Address
Returns: None
***************************************************/
void Pressure::setAddress(int myAddress){
	address=myAddress;
}
/**************************************************
* Function setBus(int)
Descritpion: Sets the I2C Bus of the pressure sensor
Args: myBus - I2C Bus
Returns: None
***************************************************/
void Pressure::setBus(int myBus){
	bus=myBus;
}
/**************************************************
* Function setSpan(int,int)
Descritpion: Sets the Span of the pressure sensor
Args: min - Min Value
      max - Max Value
Returns: None
***************************************************/
void Pressure::setSpan(int min, int max){
	pmin=min;
	pmax=max;
}
/**************************************************
* Function setFd()
Descritpion: Sets the Transducer Format Descirptor
Value
Args: fd - format desciriptor integer
Returns: None
***************************************************/
void Pressure::setFd(){

	if(bus==0){
	fd = wiringPiI2CSetupInterface("/dev/i2c-0", address);
	}
	else if (bus==1){
	fd = wiringPiI2CSetupInterface("/dev/i2c-1", address);
	}
	else{
		printf("ERROR - I2C Bus Not 0 or 1");
	}

}
/**************************************************
* Function getFd()
Descritpion: Sets the Transducer Format Descirptor
Value
Args: fd - format desciriptor integer
Returns: None
***************************************************/
int Pressure::getFd(){
	return fd;
}
/**************************************************
* Function setCounts()
Descritpion: Sets the Min and Max ADC Counts
of the pressure sensor
Args: min - Min Value
      max - Max Value
Returns: None
***************************************************/
void Pressure::setCounts(int min, int max){
	countsmin=min;
	countsmax=max;
}
/**************************************************
* Function getAddress()
Descritpion: Access the private address data
Returns: Presure Sensor Current I2C bus address
***************************************************/
int Pressure::getAddress(){
	return address;
}
/**************************************************
* Function getBus()
Descritpion: Access the private bus number data
Returns: Presure Sensor Current I2C bus ID
***************************************************/
int Pressure::getBus(){
	return bus;
}
/**************************************************
* Function getCountsMin()
Descritpion: Access the private min counts data
Returns: Presure Sensor Current min ADC Counts
***************************************************/
int Pressure::getCountsMin(){
	return countsmin;
}
/**************************************************
* Function getCountsMax()
Descritpion: Access the private min counts data
Returns: Presure Sensor Current min ADC Counts
***************************************************/
int Pressure::getCountsMax(){
	return countsmax;
}
/**************************************************
* Function getPressureMin()
Descritpion: Access the private min pressure data
Returns: Presure Sensor Current min pressure
***************************************************/
int Pressure::getPressureMin(){
	return pmin;
}
/**************************************************
* Function getPressureMax()
Descritpion: Access the private min pressure data
Returns: Presure Sensor Current min pressure
***************************************************/
int Pressure::getPressureMax(){
	return pmax;
}

/**************************************************
* Function readCounts()
Descritpion: Reads the current pressure sensor data
register.
Returns: Presure Sensor Current ADC Counts
***************************************************/
int Pressure::readCounts(){
int data, adCounts;
	data=wiringPiI2CReadReg16(fd,DATA_REG); //read 16 bits from the data register

	/*the 2 bytes that come out of the data register are reversed(Little Endian).
	so i flip them here by doing a bitwise AND for each byte, bitshfting them each 8 places
	and then OR them together to get the data register in the correct order*/
	adCounts=((data & 0x00FF) << 8) | ((data & 0xFF00) >> 8);
	return adCounts;
}
/**************************************************
* Function readPressure_mmhg()
Descritpion: Reads the current pressure sensor data
register and performs the ADC to mmHg conversion.

Returns: Presure Sensor Current Value in mmHg
***************************************************/
float Pressure::readPressure_mmhg(){
int data;
	return (counts2mmhg(readCounts()));
}
/**************************************************
* Function readPressure_mmhg()
Descritpion: Reads the current pressure sensor data
register and performs the ADC to mBar conversion.

Returns: Presure Sensor Current Value in mBar
***************************************************/
float Pressure::readPressure_mbar(){

	return (counts2mbar(readCounts()));
}
