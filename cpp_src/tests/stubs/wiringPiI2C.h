#pragma once

extern int test_read_result;
inline int wiringPiI2CSetupInterface(const char*, int) { return 7; }
inline int wiringPiI2CReadReg16(int, int) { return test_read_result; }
