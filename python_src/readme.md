# Honeywell ABPD Pressure Transducer Driver - Python Implementation

This is intended to be a lightweight module for reading a pressure transducer easily with python.  
This example is intended for developers more comfortable with python for simple reads and writes, its performance is not as robust as the C++ implementation included in this repository, but if you are just looking for some low rate data samples, this is a good starting point for anyone.

## Usage

### Example Code Usage
- After the above are installed, you can connect the sensor to your raspberry pi on bus 1.
- Deploy the code to you raspberry pi target.
- run example.py to read pressure from a single pressure sensor and write data out to a csv.

## Class Pressure
Full Documentation Coming Soon

### Attributes
- address - i2c Address of the sensor - Default Address -0x28
- minCounts - Minimum Counts (Zero Reading) for the sensor - Default Value -0x0666
- maxCounts - Maximum Counts (Full Scale Reading) for the sensor - Default Value -0x03999
- pressureMin - Minimum Pressure (Zero Reading) for the sensor - Default Value -0 mBar
- pressureMin - Maximum Pressure (Full Scale Reading) for the sensor - Default Value - 100 mBar
- bus - and smbus Object created for the i2c bus selected - default i2c Bus is 1

### Data Read Methods

The following member methods are used for reading data from the sensor.

#### readCounts()
Reads Counts from the I2C Object over the I2C bus. Reads 2 Bytes from the Data Register, but we perform and endian Swap on the two data samples with a bit shift and OR.  Raw count readings can be used to troubleshoot a faulty sensor or calibrate for an offset if needed.

- Args - None
- Returns - ADC Counts

#### readPressureMbar()
Takes a Pressure Reading from the sensor object and returns the pressure reading in mBar as a floating point value.
- Args - None
- Returns - Pressure in mBar (Float)

#### readPressureMmhg()
Takes a Pressure Reading from the sensor object and returns the pressure reading in mmHg as a floating point value.
- Args - None
- Returns - Pressure in mmHg (Float)

### Utility Methods

Some utility methods have been provided for unit conversions for the sensor

#### counts2mbar(counts)
Converts raw sensor ADC counts to Pressure in mBar by applying the following expression:

(((counts - minCounts) * (pressureMax - pressureMin)) / (maxCounts - minCounts)) + pressureMin

- Args - Counts - raw sensor counts (int)
- Returns - Pressure in mBar (float)

#### mBar2mmhg(pMbar)
Converts Pressure in mBar to mmHg with the converion ratio of 0.75
- Args - pMbar - Pressure in mBar (float)
- Returns - Pressure in mmHg (Float)



-----------------------------------------------------------