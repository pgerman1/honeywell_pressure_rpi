#pragma once

extern int test_read_result;
extern int test_close_count;
extern int test_open_result;
extern int test_open_count;
extern int test_last_closed;
inline int gpioInitialise() { return 0; }
inline int i2cOpen(unsigned, unsigned, unsigned) { ++test_open_count; return test_open_result; }
inline int i2cReadWordData(unsigned, unsigned) { return test_read_result; }
inline int i2cClose(unsigned handle) { test_last_closed = static_cast<int>(handle); ++test_close_count; return 0; }
