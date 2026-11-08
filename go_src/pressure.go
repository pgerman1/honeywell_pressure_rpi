package main

import (
	"fmt"
	"math"
	"time"

	"golang.org/x/exp/io/i2c"
)

// Constants
const (
	// mBar to mmHg conversion Ratio
	MMHG_MBAR_RATIO = 0.7500

	// Define Device min and max ADC counts
	ADC_MIN_COUNTS = 0x0666
	ADC_MAX_COUNTS = 0x3999

	// Default sensor configuration
	DEFAULT_ADDR         = 0x28
	DEFAULT_MAX_PRESSURE = 100
	DEFAULT_MIN_PRESSURE = 0
	DEFAULT_BUS          = 1
)

// Class Declration
type Pressure struct {
	bus       int        // the i2c Bus of the sensor
	address   int        // i2c Address of the sensor
	minCounts int        // Minimum Counts (Zero Reading) for the sensor
	maxCounts int        // Maximum Counts (Full Scale Reading) for the sensor
	pMin      int        // Minimum Pressure (Zero Reading) for the sensor
	pMax      int        // Maximum Pressure (Full Scale Reading) for the sensor
	i2cHandle uint32     // Reference to the I2C Handle
	data      float32    // Stored Data
	databus   i2c.Device //Create an I2C Device Variable
}

//Setter Functions

/*
*************************************************
Function set_bus(int)
Descritpion: Sets the I2C Bus of the pressure sensor
Args: myBus - I2C Bus
Returns: None
**************************************************
*/
func (sensor *Pressure) set_bus(myBus int) {
	sensor.bus = myBus
}

/*
*************************************************
Function set_address(int)
Descritpion: Sets the I2C Address of the pressure sensor
Args: myAddr - I2C Address
Returns: None
**************************************************
*/
func (sensor *Pressure) set_address(myAddr int) {
	sensor.address = myAddr
}

/*
*************************************************
Function set_min_counts(int)
Descritpion: Sets the Minumum Counts of pressure sensor
Args: counts- min ADC Counts from Sensor Data Sheet.
Returns: None
**************************************************
*/
func (sensor *Pressure) set_min_counts(myCounts int) {
	sensor.minCounts = myCounts
}

/*
*************************************************
Function set_max_counts(int)
Descritpion: Sets the maximum counts of pressure sensor
Args: counts- max ADC Counts from Sensor Data Sheet.
Returns: None
**************************************************
*/
func (sensor *Pressure) set_max_counts(myCounts int) {
	sensor.maxCounts = myCounts
}

/*
*************************************************
Function set_max_pressure(int)
Descritpion: Sets the maximum pressure range of pressure sensor
in mBar
Args: counts- max pressure from Sensor Data Sheet.
Returns: None
**************************************************
*/
func (sensor *Pressure) set_max_pressure(myPressure int) {
	sensor.pMax = myPressure
}

/*
*************************************************
Function set_max_pressure(int)
Descritpion: Sets the maximum pressure range of pressure sensor
in mBar
Args: counts- max pressure from Sensor Data Sheet.
Returns: None
**************************************************
*/
func (sensor *Pressure) set_min_pressure(myPressure int) {
	sensor.pMin = myPressure
}

// initialize opens the configured bus and address. Set both before calling it.
func (sensor *Pressure) initialize() {
	device := fmt.Sprintf("/dev/i2c-%d", sensor.bus)
	i2cdevice, err := i2c.Open(&i2c.Devfs{Dev: device}, sensor.address)
	if err != nil {
		panic(err)
	}

	sensor.databus = *i2cdevice
}

/*
*************************************************
Function read_counts()
Description: Reads two bytes in wire order and logs the raw word, status,
and 14-bit pressure counts. No register-address write is performed.
Returns: ADC counts for normal status, or -1 for non-normal status.
An I2C read error causes a panic.
**************************************************
*/
func (sensor *Pressure) read_counts() int {
	buffer := make([]byte, 2)

	if err := sensor.databus.Read(buffer); err != nil {
		panic(fmt.Errorf("I2C read failed: %w", err))
	}

	raw := uint16(buffer[0])<<8 | uint16(buffer[1])
	status := raw >> 14
	counts := int(raw & 0x3fff)

	fmt.Printf("raw=0x%04x status=%02b counts=%d\n",
		raw, status, counts)

	if status != 0 {
		// Diagnostic sentinel: do not convert this to pressure.
		return -1
	}

	return counts
}

/*
*************************************************
Function read_mbar()
Descritpion: Reads the current pressure sensor data
and converts it to milibars
Returns: Pressure in millibars (float32), or NaN for non-normal status.
**************************************************
*/
func (sensor *Pressure) read_mbar() float32 {

	counts := sensor.read_counts()
	if counts < 0 {
		return float32(math.NaN())
	}
	return sensor.counts2mbar(counts)

}

/*
*************************************************
Function read_mmhg()
Description: Reads pressure and converts it to mmHg.
Returns: Pressure in mmHg (float32), or NaN for non-normal status.
**************************************************
*/
func (sensor *Pressure) read_mmhg() float32 {

	return (sensor.read_mbar() * MMHG_MBAR_RATIO)

}

/*
*************************************************
Function counts2mbar()
Descritpion: Converts Counts to Milibar Pressure
Returns: pMbar - Pressure in Milibars (Float32)
**************************************************
*/
func (sensor *Pressure) counts2mbar(counts int) float32 {

	var pMbar float32
	num := float32(int(counts)-sensor.minCounts) * float32(sensor.pMax-sensor.pMin) // Numerator
	den := float32(sensor.maxCounts - sensor.minCounts)                             //Denominator
	pMbar = (num / den) + float32(sensor.pMin)                                      //add min pressure, should add an additional calibration offset.
	return pMbar
}

func newSensor() *Pressure {
	sensor := Pressure{}
	sensor.set_address(DEFAULT_ADDR)
	sensor.set_bus(DEFAULT_BUS)
	sensor.set_max_counts(ADC_MAX_COUNTS)
	sensor.set_min_counts(ADC_MIN_COUNTS)
	sensor.set_max_pressure(DEFAULT_MAX_PRESSURE)
	sensor.set_min_pressure(DEFAULT_MIN_PRESSURE)

	sensor.initialize() // initialize Sensor

	return &sensor
}

func main() {

	// Declare a Sensor Object
	var sensor1 = newSensor()
	defer sensor1.databus.Close()
	fmt.Printf("Sensor Bus : %d (/dev/i2c-%d), Address : 0x%02x\n",
		sensor1.bus, sensor1.bus, sensor1.address)
	skipped := 0
	for i := 0; i < 100; i++ {
		dataSample := sensor1.read_counts()
		if dataSample < 0 {
			skipped++
			fmt.Println("Skipping sample: non-normal sensor status")
		} else {
			// Convert this sample without performing another sensor read.
			mmhg := sensor1.counts2mbar(dataSample) * MMHG_MBAR_RATIO
			fmt.Printf("Pressure : %.3f mmHg\n", mmhg)
		}
		time.Sleep(100 * time.Millisecond)
	}
	fmt.Printf("Skipped samples: %d\n", skipped)
}
