# Honeywell ABPD Pressure Transducer Driver - Go Implementation

This is intended to be a demo package for reading a pressure transducer easily with the go language.  
This example required some knowlege of the go language and compiler simple reads and writes.  The eventual intent is to benchmark the performance of each of the implementations of this driver against each other and compare performance.

## Usage

### Example Code Usage
- After the above are installed, you can connect the sensor to your raspberry pi on i2c bus 1.
- Complile the code using go build. You will need to cross compile it for your particular raspberry pi hardware.
* this was originally written for the raspberry pi zero, which uses an ARMV6 Procesor
- Deploy the code to you raspberry pi target.
- run example.py to read pressure from a single pressure sensor and write data out to a csv.

## Class Pressure
Full Documentation Coming Soon

### Attributes
- address - i2c Address of the sensor - Default Address -0x28
- minCounts - Minimum Counts (Zero Reading) for the sensor - Default Value -0x0666
- maxCounts - Maximum Counts (Full Scale Reading) for the sensor - Default Value -0x03999
- pMin - Minimum Pressure (Zero Reading) for the sensor - Default Value -0 mBar
- pMin - Maximum Pressure (Full Scale Reading) for the sensor - Default Value - 100 mBar
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