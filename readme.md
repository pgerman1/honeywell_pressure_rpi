# Honeywell ABPD Pressure Transducer Driver

## Overview
This Repository Contains a pressure transducer driver for the Honeywell ABPDLNN100MG2A3 Pressure Sensor communicating over an I2C Bus on the raspberry pi.  Driver is setup to initialize the I2C Bus and read data from a working

Similar I2C Pressure Sensor models could be used with this driver with minor modifications to the Member functions of this class.

This driver was designed for the Raspberry pi using the wiringPi and wiringPiI2C Library

## Usage

### Constructors

Four constructors are provided depending on user intent.

#### Pressure();		  //base constructor
Base Constructor, Constructs a Pressure Transducer with Default Parameters

I2C Bus = 1
I2C Address = 0x28
Pressure Min = 0 mmhg
Pressure Max = 100 mmhg
setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);

#### Pressure(bus)
Constructor, Constructs a Pressure Transducer with a custom bus setting.

Pressure Min = 0 mbar
Pressure Max = 100 mBar
setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);

#### Pressure(bus,address);	  //Constructor, Setting Bus And Address
Constructor, Constructs a Pressure Transducer with a custom bus and address setting.
Pressure Min = 0 mBar
Pressure Max = 100 mBar
setCounts(ADC_MIN_COUNTS,ADC_MAX_COUNTS);

#### Pressure(bus,address,pressure min, pressure max);
Constructor, Constructs a Pressure Transducer with a custom bus, address, and pressure min/max setting.

### Methods

#### float mbar2mmhg(mBar_Pressure)
Utility Method, Conversion of mmHg Counts to mBar.
args - pressure in mBar (float)
returns - Pressure in mmHg

#### float counts2mbar(counts)
Conversion of raw ADC Counts to mBar.
args - counts (int)
returns - Pressure in mBar

#### float counts2mmhg(int)   //convert ADC Counts to mmhg
Conversion of raw ADC Counts to mmHg.
args - counts (int)
returns - Pressure in mmHg

#### void getFd();             //get format descirptor
prints the Format Descriptor of the object to the standard output.

#### void getAddress();        //return I2C Address ** Fix
prints the I2C Address of the object to the standard output.
#### void getBus();        		 //return I2C Address ** Fix
prints the bus information to the standard output.
#### int readCounts();         //Read Data in ADC Counts
Read a single data sample the sensor and returns a floating point value raw ADC Counts.
Method is best used for calibration of the sensor.

#### float readPressure_mmhg();
Read a single data sample the sensor and returns a floating point value in mmHg

#### float readPressure_mbar();//Read Data in mBar
Read a single data sample the sensor and returns a floating point value in mBar

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

##### address (int)   
I2C address of the Transducer IC.  This is set by the manufacturer for this model and can be obtained from the datasheet.  Sensors can be ordered with various address options, but are set by the factory in firmware and cannot be changed by the end user.
Default Value = 0x28

##### bus (int)       // I2C Bus of the Transducer
Raspberry pi I2C Bus used for communicating with the sensor.  Pi Models may have multiple buses, so this was left configurable.
Default Value = 1
##### fd (int)	      
Transducer Format Descriptor (WIP)
