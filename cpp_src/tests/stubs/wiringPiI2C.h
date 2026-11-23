#pragma once
#include <cerrno>
#include <fcntl.h>

extern int test_read_result;
extern int test_open_result;
extern int test_open_count;
extern int test_last_opened;
inline int wiringPiI2CSetupInterface(const char*, int) {
    ++test_open_count;
    if (test_open_result < 0) { errno = EACCES; return -1; }
    // Real disposable descriptors let tests verify actual close() behavior.
    return test_last_opened = ::open("/dev/null", O_RDONLY);
}
inline int wiringPiI2CReadReg16(int, int) { return test_read_result; }
