#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <type_traits>
#include "pressure.h"

int test_read_result = 0;
int test_close_count = 0;

void set_frame(unsigned frame) {
    test_read_result = ((frame & 0xff) << 8) | (frame >> 8);
}

template<typename Read>
void expect_error(Read read, const char* message) {
    bool caught = false;
    try {
        read();
    } catch (const std::runtime_error& error) {
        caught = true;
        assert(std::string(error.what()).find(message) != std::string::npos);
    }
    assert(caught);
}

void expect_read_errors(Pressure& sensor, const char* message) {
    expect_error([&] { sensor.readCounts(); }, message);
    expect_error([&] { sensor.readPressure_mbar(); }, message);
    expect_error([&] { sensor.readPressure_mmhg(); }, message);
}

int main() {
#ifdef TEST_PIGPIO
    static_assert(!std::is_copy_constructible<Pressure>::value, "Handle must not be copied");
    static_assert(!std::is_copy_assignable<Pressure>::value, "Handle must not be assigned");
    static_assert(!std::is_move_constructible<Pressure>::value, "Moves are not supported");
    static_assert(!std::is_move_assignable<Pressure>::value, "Moves are not supported");
#endif
    {
        Pressure sensor;
        // Cover every valid 14-bit payload, including both boundaries.
        for (unsigned counts = 0; counts <= 0x3fff; ++counts) {
            set_frame(counts);
            assert(sensor.readCounts() == static_cast<int>(counts));
        }
        set_frame(0x0666);
        assert(sensor.readPressure_mbar() == 0.0f);
        assert(sensor.readPressure_mmhg() == 0.0f);
        set_frame(0x3999);
        assert(std::fabs(sensor.readPressure_mbar() - 100.0f) < 0.001f);
        assert(std::fabs(sensor.readPressure_mmhg() - 75.0f) < 0.001f);

        for (int error : {-1, -24, -83}) {
            test_read_result = error;
            expect_read_errors(sensor, "I2C pressure read failed");
        }
        const char* messages[] = {"", "command mode", "stale data", "diagnostic condition"};
        for (unsigned status = 1; status <= 3; ++status) {
            for (unsigned counts : {0u, 0x0666u, 0x3999u, 0x3fffu}) {
                set_frame((status << 14) | counts);
                expect_read_errors(sensor, messages[status]);
            }
        }
        // A failed sample does not prevent a subsequent successful read.
        set_frame(0x0666);
        assert(sensor.readCounts() == 0x0666);
        assert(test_close_count == 0);
    }
#ifdef TEST_PIGPIO
    assert(test_close_count == 1);
#endif
    std::cout << "Pressure regression checks passed\n";
}
