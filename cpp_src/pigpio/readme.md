# Honeywell ABPD Pressure Transducer Driver

## Overview
This Repository Contains a pressure transducer driver for the Honeywell ABPDLNN100MG2A3 Pressure Sensor communicating over an I2C Bus on the raspberry pi.  Driver is setup to initialize the I2C Bus and read data from an installed and powered sensor. The Sensor used for the development driver is a 100 mBar range sensor

Similar I2C Pressure Sensor models could be used with this driver with minor modifications to the Member functions of this class.

Example Datasheet : https://www.mouser.com/datasheet/2/187/HWSC_S_A0013047928_1-3073376.pdf

This driver was originally designed for the Raspberry pi using the wiringPi and wiringPiI2C Library and was ported to the pigpio library on 4/1/2023.

## Dependencies

For the examples to work out of the box you will need the the following installed:
- g++
- make
- pigpio (https://abyz.me.uk/rpi/pigpio/index.html)
- Ensure that I2C is enabled on your Raspberry Pi (use raspi-config to enable)

## Usage

### Example Code Usage
- After the above are installed, you can connect the sensor to your raspberry pi on bus 1.
- Deploy the code to you raspberry pi target.
- compile with make.
- run example_single to read pressure from a single pressure sensor.

-----------------------------------------------------------

### Constructors

Four constructors are provided depending on user intent.

#### Pressure()
Base Constructor, Constructs a Pressure Transducer with Default Parameters and opens an I2C Reference (handle) to the sensor.

I2C Bus = 1
I2C Address = 0x28
Pressure Min = 0 mmhg
Pressure Max = 100 mmhg
setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);

#### Pressure(bus)
Constructor, Constructs a Pressure Transducer with a custom bus setting and opens an I2C Reference (handle) to the sensor.

Pressure Min = 0 mbar
Pressure Max = 100 mBar
setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);

#### Pressure(bus,address);
Constructor, Constructs a Pressure Transducer with a custom bus and address setting.  This Constructor also opens an I2C Reference (handle) to the sensor.
Pressure Min = 0 mBar
Pressure Max = 100 mBar
setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);

#### Pressure(bus,address,pressure min, pressure max);
Constructor, Constructs a Pressure Transducer with a custom bus, address, and pressure min/max setting.This Constructor also opens an I2C Reference (handle) to the sensor.

-----------------------------------------------------------

### Destructor
#### ~Pressure();
Destructor Closes the I2C reference from the sensor object and releases resouces.

-----------------------------------------------------------

### Methods - Getters

#### unsigned getAddress(void)
return the I2C address of the sensor object

#### unsigned getBus(void)
return the I2C Bus Number of the sensor object

#### int getCountsMin(void)
return the Minimum Number of I2C Counts of the sensor object.  For the default application this number of counts coresponds to 0 mBar.

#### int getCountsMax(void)
return the Maximum Number of I2C Counts of the sensor object.  For the default application this number of counts coresponds to 100 mBar.

#### int getPressureMin(void)
return the Minimum Pressure Scale of the sensor object.  For the default application this will be 0 mBar.

#### int getPressureMax(void)
return the Maximum Pressure Scale of the sensor object.  For the default application this will be 100 mBar.

#### unsigned getHandle(void)
return the I2C reference Handle of sensor object.  Useful for Troubleshooting.

-----------------------------------------------------------

### Methods - Setters

#### void setCounts(int, int);            
Sets the Max and Min Counts for the Sensor Object.  Default is 0x0666 and 0x3999. Used when constructing a new object.
- Args
-- int minCounts, int maxCounts
-- Returns - None

#### void setSpan(int, int);            
Sets the Max and Min Pressure readings for the Sensor Object.  Default is 0 and 100. Used when constructing a new object.
- Args
* int minCounts, int maxCounts
- Returns 
* None

#### void setAddress(unsigned);            
Sets the addres of the sensor object to the value of the new argument passed.  Used when constructing a new object.
- Args 
* unsigned i2cAdress
- Returns 
* None

#### void setBus(unsigned);            
Sets the I2C bus of the sensor object to the value of the new argument passed.  Used when constructing a new object.
- Args 
* unsigned i2cBus
- Returns 
* None

#### void setHandle();            
uses the i2cOpen method to open an I2C refernce to the adress of the object.  Should be called after setting the I2C address.
Args - none
Returns - None

-----------------------------------------------------------

### Methods - I/O Readers

#### int readCounts(); 
Read a single data sample the sensor and returns a floating point value raw ADC Counts.  Performs Endian Swap when performing 16 bit data register read. Method is best used for calibration of the sensor.
- args - None
- reutrns - Raw ADC Counts

#### float readPressure_mmhg();
Read a single data sample the sensor and returns a floating point value in mmHg.
- args - None
- reutrns - Pressure Reading in mmHg

#### float readPressure_mbar()
Read a single data sample the sensor and returns a floating point value in mBar
- args - None
- reutrns - Pressure Reading in mmHg

-----------------------------------------------------------

### Methods - Utility


#### float mbar2mmhg(mBar_Pressure)
Utility Method, Conversion of mmHg Counts to mBar.
args - pressure in mBar (float)
returns - Pressure in mmHg

#### float counts2mbar(counts)
Conversion of raw ADC Counts to mBar.
args - counts (int)
returns - Pressure in mBar

#### float counts2mmhg(int) 
Conversion of raw ADC Counts to mmHg.
args - counts (int)
returns - Pressure in mmHg

--------------------------------------------------

### Object Atributes
Object Atributes are set in the constructor or can be set and retrieved using the Methods above, but are private data for the class.

##### int countsmin (int)
Minimum ADC Counts of the Sensor, this has a default value of 0x0666

##### countsmax (int)
Minimum ADC Counts of the Sensor, this has a default value of 0x3999

##### pmin (int)      
Minimum Pressure for sensor rannge (mBar)
Default Value = 0

##### pmax (int)      
Minimum Pressure for sensor rannge (mBar)
Default Value = 0

##### address (unsigned)   
I2C address of the Transducer IC.  This is set by the manufacturer for this model and can be obtained from the datasheet.  Sensors can be ordered with various address options, but are set by the factory in firmware and cannot be changed by the end user.
Default Value = 0x28

##### bus (unsigned)       // I2C Bus of the Transducer
Raspberry pi I2C Bus used for communicating with the sensor.  Pi Models may have multiple buses, so this was left configurable.
Default Value = 1
##### handle (unsigned)	      
Handle created by the i2cOpen method, this is passed as a reference to the I2C read Functions.
