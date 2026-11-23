CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Wpedantic -g -fsanitize=undefined -fno-sanitize-recover=undefined
BUILD_DIR ?= /tmp/honeywell-pressure-cpp-tests

.PHONY: test
test:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -Istubs -I../pigpio/include -DTEST_PIGPIO pressure_test.cpp -o $(BUILD_DIR)/pigpio_test
	$(BUILD_DIR)/pigpio_test
	$(CXX) $(CXXFLAGS) -Istubs -I../wiringpi pressure_test.cpp -o $(BUILD_DIR)/wiringpi_test
	$(BUILD_DIR)/wiringpi_test
	$(CXX) $(CXXFLAGS) -Istubs -I../pigpio/include -fsyntax-only ../pigpio/src/example.cpp ../pigpio/src/example_double.cpp
	$(CXX) $(CXXFLAGS) -Istubs -I../wiringpi -fsyntax-only ../wiringpi/example.cpp ../wiringpi/example_double.cpp
