#include <cassert>
#include <climits>
#include <cmath>
#include <iostream>
#include <string>
#include <type_traits>
#include "pressure.h"

int test_read_result = 0;
int test_close_count = 0;
int test_open_result = 7;
int test_open_count = 0;
int test_last_closed = -1;
int test_last_opened = -1;

void set_frame(unsigned frame) {
    test_read_result = ((frame & 0xff) << 8) | (frame >> 8);
}

template<typename Error = std::runtime_error, typename Read>
void expect_error(Read read, const char* message) {
    bool caught = false;
    try {
        read();
    } catch (const Error& error) {
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
    static_assert(!std::is_copy_constructible<Pressure>::value, "Handle must not be copied");
    static_assert(!std::is_copy_assignable<Pressure>::value, "Handle must not be assigned");
    static_assert(!std::is_move_constructible<Pressure>::value, "Moves are not supported");
    static_assert(!std::is_move_assignable<Pressure>::value, "Moves are not supported");
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
#ifndef TEST_PIGPIO
    assert(::fcntl(test_last_opened, F_GETFD) == -1 && errno == EBADF);
#endif
    // All configuration validation happens before any resource is acquired.
    const int opens_before = test_open_count;
    const int closes_before = test_close_count;
    for (int bus : {-1, 2, INT_MAX}) {
        expect_error<std::invalid_argument>([&] { Pressure sensor(bus); }, "bus");
    }
    for (int address : {-1, 0, 7, 0x78, 0x80, INT_MAX}) {
        expect_error<std::invalid_argument>([&] { Pressure sensor(1, address); }, "address");
    }
    expect_error<std::invalid_argument>([] { Pressure sensor(1, 0x28, 0, 0); }, "Minimum pressure");
    expect_error<std::invalid_argument>([] { Pressure sensor(1, 0x28, 100, 0); }, "Minimum pressure");
    assert(test_open_count == opens_before);
    assert(test_close_count == closes_before);

    // Every constructor must propagate open failure without closing anything.
    for (int failure : {-1, -24, -83}) {
        test_open_result = failure;
        expect_error([] { Pressure sensor; }, "open failed");
        expect_error([] { Pressure sensor(0); }, "open failed");
        expect_error([] { Pressure sensor(1, 0x28); }, "open failed");
        expect_error([] { Pressure sensor(1, 0x28, -100, 100); }, "open failed");
    }
    assert(test_open_count == opens_before + 12);
    assert(test_close_count == closes_before);
    test_open_result = 7;

    // Repeated scope exit releases resources, including exception unwinding.
    for (int i = 0; i < 100; ++i) {
        const int before = test_close_count;
        expect_error([] { Pressure sensor; throw std::runtime_error("unwind"); }, "unwind");
#ifdef TEST_PIGPIO
        assert(test_close_count == before + 1);
        assert(test_last_closed == 7);
#else
        (void)before;
        assert(::fcntl(test_last_opened, F_GETFD) == -1 && errno == EBADF);
#endif
    }
#ifdef TEST_PIGPIO
    // Zero is also a valid handle and must be closed.
    test_open_result = 0;
    { Pressure sensor; assert(sensor.getHandle() == 0); }
    assert(test_last_closed == 0);
    test_open_result = 7;
#endif
    for (int bus : {0, 1}) {
        for (int address : {0x08, 0x77}) {
            Pressure sensor(bus, address);
            assert(sensor.getBus() == static_cast<decltype(sensor.getBus())>(bus));
            assert(sensor.getAddress() == static_cast<decltype(sensor.getAddress())>(address));
        }
    }
    {
        Pressure sensor(1, 0x28, INT_MIN, INT_MAX);
        assert(sensor.counts2mbar(0x0666) == static_cast<float>(INT_MIN));
        assert(sensor.counts2mbar(0x3999) == static_cast<float>(INT_MAX));
        for (int counts : {INT_MIN, 0, 8192, 16383, INT_MAX}) {
            const double expected = ((static_cast<double>(counts) - 1638) *
                (static_cast<double>(INT_MAX) - INT_MIN) / 13107) + INT_MIN;
            assert(sensor.counts2mbar(counts) == static_cast<float>(expected));
            assert(std::isfinite(sensor.counts2mmhg(counts)));
        }
    }
    {
        Pressure sensor(1, 0x28, -100, 100);
        assert(sensor.counts2mbar(0x0666) == -100.0f);
        assert(sensor.counts2mbar(0x3999) == 100.0f);
    }
    std::cout << "Pressure regression checks passed\n";
}
