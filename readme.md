# Honeywell ABPD Pressure Transducer Driver

## Overview
This Repository Contains a pressure transducer driver for the Honeywell ABPDLNN100MG2A3 Pressure Sensor communicating over an I2C Bus on the raspberry pi.  Driver is setup to initialize the I2C Bus and read data from an installed and powered sensor. The Sensor used for the development driver is a 100 mBar range sensor

Similar I2C Pressure Sensor models could be used with this driver with minor modifications to the Member functions of this class.

Example Datasheet : https://www.mouser.com/datasheet/2/187/HWSC_S_A0013047928_1-3073376.pdf

This driver was originally designed for the Raspberry pi using the wiringPi and wiringPiI2C Library.

Update 4/1/2023 - Wiring pi library is deprecated by its owner, so the API has been ported to use pigpio instead.  This is maintained at https://abyz.me.uk/rpi/pigpio/

Update 4/3/2023 - Python Implementation examples have been added and tested, Full Documentation Coming Soon.

## Implementations

| Language | Location | Notes |
| --- | --- | --- |
| Rust | [Documentation](rust_src/readme.md) | I²C driver using RPPAL, with status checking and unit tests |
| C++ (pigpio) | [Documentation](cpp_src/pigpio/readme.md) | Working C++ implementation; requires g++, make, and pigpio |
| C++ (wiringPi) | [Documentation](cpp_src/wiringpi/readme.md) | Original implementation |
| Python | [Documentation](python_src/readme.md) | Limited implementation for prototyping |
