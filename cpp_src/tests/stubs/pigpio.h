#pragma once

extern int test_read_result;
extern int test_close_count;
inline int gpioInitialise() { return 0; }
inline int i2cOpen(unsigned, unsigned, unsigned) { return 7; }
inline int i2cReadWordData(unsigned, unsigned) { return test_read_result; }
inline int i2cClose(unsigned) { ++test_close_count; return 0; }
