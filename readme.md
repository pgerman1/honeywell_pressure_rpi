# Honeywell ABPD Pressure Transducer Driver

## Overview
This Repository Contains a pressure transducer driver for the Honeywell ABPDLNN100MG2A3 Pressure Sensor communicating over an I2C Bus on the raspberry pi.  Driver is setup to initialize the I2C Bus and read data from an installed and powered sensor. The Sensor used for the development driver is a 100 mBar range sensor

Similar I2C Pressure Sensor models could be used with this driver with minor modifications to the Member functions of this class.

Example Datasheet : https://www.mouser.com/datasheet/2/187/HWSC_S_A0013047928_1-3073376.pdf

This driver was originally designed for the Raspberry pi using the wiringPi and wiringPiI2C Library.

Update 4/1/2023 - Wiring pi library is deprecated by its owner, so the API has been ported to use pigpio instead.  This is maintained at https://abyz.me.uk/rpi/pigpio/

Update 4/3/2023 - Python Implementation examples have been added and tested, Full Documentation Coming Soon.

Update 11/8/2026 - Go and Rust Implementation examples have been added and tested.

## Implementations

| Language | Location | Notes |
| --- | --- | --- |
| Go | [Documentation](go_src/readme.md) | Two-byte I²C reads with status checking, raw diagnostics, and converted mmHg output |
| Rust | [Documentation](rust_src/readme.md) | I²C driver using RPPAL, with status checking and unit tests |
| C++ (pigpio) | [Documentation](cpp_src/pigpio/readme.md) | Working C++ implementation; requires g++, make, and pigpio |
| C++ (wiringPi) | [Documentation](cpp_src/wiringpi/readme.md) | Original implementation |
| Python | [Documentation](python_src/readme.md) | Limited implementation for prototyping |

## C++ error handling and handle ownership

The C++ drivers now include these fixes:

- **Read failures:** Both pigpio and wiringPi check negative I²C read results before decoding. Failed reads throw `std::runtime_error` instead of being converted into pressure values.
- **Sensor status:** Both drivers check the upper two status bits and return only the lower 14 pressure bits for normal samples. Command mode, stale data, and diagnostic conditions throw `std::runtime_error` with a descriptive message. There is no automatic retry.
- **Handle ownership and cleanup:** Both backends' `Pressure` objects cannot be copied or moved. Each object closes its successfully opened resource on destruction, including exception unwinding. Pass sensors by reference; all example helpers now do this. `getHandle()` and `getFd()` expose borrowed identifiers: callers must not close them.
- **Initialization validation:** Constructors reject buses other than 0 or 1, addresses outside the non-reserved 7-bit range `0x08`–`0x77`, and pressure spans whose minimum is not less than the maximum. Invalid configuration throws `std::invalid_argument` before opening the device. Negative open results throw `std::runtime_error` (a `std::system_error` preserving `errno` for wiringPi). Internal resource identifiers start at `-1`; failed opens are never stored as unsigned handles or closed.
- **Pressure-range arithmetic:** Conversion promotes operands to `double` before subtracting the pressure endpoints, preventing signed integer overflow even for an `INT_MIN`–`INT_MAX` span. Negative pressure ranges remain supported; the public return type is still `float`.

The signatures of `readCounts()`, `readPressure_mbar()`, and `readPressure_mmhg()` are unchanged, but callers must now handle exceptions. The examples catch `std::exception` for both configuration and runtime failures, print the error and exit with a nonzero status. Applications can catch `std::runtime_error` around a read and decide whether to stop or retry; failed samples must not be used as measurements. C++11 or newer is required and is selected by the example makefiles.

Run the hardware-independent regression checks with:

```sh
make -C cpp_src/tests test
```

These checks use stubbed I²C calls and UndefinedBehaviorSanitizer to cover normal counts, read failures, all non-normal statuses, pressure-method error propagation, ownership and cleanup for both backends, initialization failures, invalid configuration, and extreme pressure ranges. They also compile-check the examples. Raspberry Pi hardware validation is still required. Changing the SMBus reads to direct I²C reads remains follow-up work.
