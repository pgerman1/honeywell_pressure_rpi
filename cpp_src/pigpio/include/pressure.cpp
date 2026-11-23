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
	// Promote before subtraction: even a valid signed pressure span may
	// exceed the range of int (for example INT_MIN through INT_MAX).
	const double span = static_cast<double>(pmax) - static_cast<double>(pmin);
	return static_cast<float>(((static_cast<double>(counts) - countsmin) * span /
	                          (static_cast<double>(countsmax) - countsmin)) + pmin);
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
	if (myAddress < 0x08 || myAddress > 0x77) {
		throw std::invalid_argument("I2C address must be a non-reserved 7-bit address (0x08-0x77)");
	}
	address=myAddress;
}
/**************************************************
* Function setBus(int)
Descritpion: Sets the I2C Bus of the pressure sensor
Args: myBus - I2C Bus 
Returns: None
***************************************************/
void Pressure::setBus(unsigned myBus){
	if (myBus != 0 && myBus != 1) {
		throw std::invalid_argument("I2C bus must be 0 or 1");
	}
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
	if (min >= max) {
		throw std::invalid_argument("Minimum pressure must be less than maximum pressure");
	}
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
	const int opened = i2cOpen(bus,address,0);
	if (opened < 0) {
		throw std::runtime_error("I2C pressure open failed (code " +
		                         std::to_string(opened) + ")");
	}
	handle = opened;
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
	const int data = i2cReadWordData(handle,DATA_REG);
	if (data < 0) {
		throw std::runtime_error("I2C pressure read failed (code " +
		                         std::to_string(data) + ")");
	}

	// The SMBus word read returns the two sensor bytes in reverse order.
	const unsigned raw = ((data & 0x00FF) << 8) | ((data & 0xFF00) >> 8);
	switch (raw >> 14) {
	case NORMAL_OPERATION:
		return static_cast<int>(raw & 0x3FFF);
	case COMMAND_MODE:
		throw std::runtime_error("Pressure sensor is in command mode");
	case STALE_DATA:
		throw std::runtime_error("Pressure sensor returned stale data");
	case DIAG_CONDITION:
		throw std::runtime_error("Pressure sensor reported a diagnostic condition");
	default:
		throw std::runtime_error("Unknown pressure sensor status");
	}
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
	if (handle >= 0) {
		i2cClose(static_cast<unsigned>(handle));
	}
}
