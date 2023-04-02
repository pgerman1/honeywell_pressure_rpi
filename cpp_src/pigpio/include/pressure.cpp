#include "pressure.h"

using namespace std;

/**************************************************
* Constructor Method
Creates a Default Sensor
-----------------------------------
Address = 0x28
Bus = /dev/i2c-1
Pressure Span = 0-100 mBar
Min ADC counts = 0x0666
Max ADC counts = 0x3999
-----------------------------------
Args: I2C Bus Number
Returns: New Pressure Sensor Object
***************************************************/

Pressure::Pressure(){
	setBus(1);
	setAddress(DEFAULT_ADDRESS);
	setSpan(0,100);
	setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);
	setHandle();
}

/**************************************************
* Constructor Method
Creates a Default Sensor, I2C Bus Declared
-----------------------------------
Address = 0x28
Bus = /dev/i2c-1
Pressure Span = 0-100 mBar
Min ADC counts = 0x0666
Max ADC counts = 0x3999
-----------------------------------
Args: I2C Bus Number
Returns: New Pressure Sensor Object
***************************************************/

Pressure::Pressure(unsigned myBus){
	setBus(myBus);
	setAddress(DEFAULT_ADDRESS);
	setSpan(0,100);
	setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);
	setHandle();
}

/**************************************************
* Constructor Method
Creates a Default Sensor, I2C Bus Declared, user-defined address
-----------------------------------
Pressure Span = 0-100 mBar
Min ADC counts = 0x0666
Max ADC counts = 0x3999
-----------------------------------
Args: I2C Bus Number
Returns: New Pressure Sensor Object
***************************************************/

Pressure::Pressure(unsigned myBus, unsigned myAddress){
	setBus(myBus);
	setAddress(myAddress);
	setSpan(0,100);
	setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);
	setHandle();
}

/**************************************************
* Constructor Method
Creates a Default Sensor, I2C Bus Declared, user-defined address,
User Defined Span
-----------------------------------
Min ADC counts = 0x0666
Max ADC counts = 0x3999
-----------------------------------
Args: I2C Bus Number, address, span min, span max
Returns: New Pressure Sensor Object
***************************************************/

Pressure::Pressure(unsigned myBus, unsigned myAddress, int spanMin, int spanMax){
	setBus(myBus);
	setAddress(myAddress);
	setSpan(spanMin,spanMax);
	setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);
	setHandle();
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

	return (counts2mbar(counts)*MMHG_MBAR_RATIO);
}

/**************************************************
* Function mbar2mmhg()
Descritpion: Converts Pressure Transducer ADC Counts
to pressure in millibars.
Args: p_mbar - 
Returns: pressure in mmHg
***************************************************/
float Pressure::mbar2mmhg(float p_mbar){

	return p_mbar*MMHG_MBAR_RATIO;  // .75 factor conversion
}
/**************************************************
* Function setAddress()
Descritpion: Sets the I2C Address of the pressure sensor
Args: myAddress - I2C Address 
Returns: None
***************************************************/
void Pressure::setAddress(unsigned myAddress){
	address=myAddress;
}
/**************************************************
* Function setBus(int)
Descritpion: Sets the I2C Bus of the pressure sensor
Args: myBus - I2C Bus 
Returns: None
***************************************************/
void Pressure::setBus(unsigned myBus){
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
* Function sethandle(int)
Descritpion: Sets the Transducer Format Descirptor
Value
Args: handle - format desciriptor integer
Returns: None
***************************************************/
unsigned Pressure::getHandle(){
	return handle;
}
/**************************************************
* Function sethandle(int)
Descritpion: Sets the Transducer Format Descirptor
Value
Args: handle - format desciriptor integer
Returns: None
***************************************************/
void Pressure::setHandle(){
	handle = i2cOpen(bus,address,0);  // Third Arg hard set to 0, no i2c Flags
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
* Function readCounts()
Descritpion: Reads the current pressure sensor data
register. 
Returns: Presure Sensor Current ADC Counts
***************************************************/
int Pressure::readCounts(){
int data, adCounts;
	data=i2cReadWordData(handle,DATA_REG);
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
	data=readCounts();
	return (counts2mmhg(data));
}
/**************************************************
* Function readPressure_mmhg()
Descritpion: Reads the current pressure sensor data
register and performs the ADC to mBar conversion.

Returns: Presure Sensor Current Value in mBar
***************************************************/
float Pressure::readPressure_mbar(){
int data;
	data=readCounts();
	return (counts2mbar(data));
}
/**************************************************
* Function getAddress()
Descritpion: Return the address of the Object

Returns: address - unsigned
***************************************************/
unsigned Pressure::getAddress(){
	return address;
}
/**************************************************
* Function bus()
Descritpion: Return the bus of the Object

Returns: bus - unsigned
***************************************************/
unsigned Pressure::getBus(){
	return bus;
}
/**************************************************
* Function getCountsMin()
Descritpion: Return the min ADC Counts of the Object

Returns: Min Counts - int
***************************************************/
int Pressure::getCountsMin(){
	return countsmin;
}
/**************************************************
* Function getCountsMax()
Descritpion: Return the min ADC Counts of the Object

Returns: Max Counts - int
***************************************************/
int Pressure::getCountsMax(){
	return countsmax;
}


/**************************************************
* Function getCountsMin()
Descritpion: Return the min ADC Counts of the Object

Returns: Min Counts - int
***************************************************/
int Pressure::getPressureMin(){
	return pmin;
}
/**************************************************
* Function getCountsMax()
Descritpion: Return the min ADC Counts of the Object

Returns: Max Counts - int
***************************************************/
int Pressure::getPressureMax(){
	return pmax;
}

/**************************************************
* Function ~Pressure()
Descritpion: Destructor Method, Close Open References
To I2C Busses when resouces are released
Args : none
Returns: None
***************************************************/
Pressure::~Pressure(){
	i2cClose(getHandle());  // Close the I2C Bus for the objects Handle.
}
