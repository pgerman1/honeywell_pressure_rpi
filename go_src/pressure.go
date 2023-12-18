package main

import (
	"bytes"
	"fmt"
	"strconv"
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

	// Define Device Data Register Value
	DATA_REG             = 0x00
	DEFAULT_ADDR         = 0x28
	DEFAULT_MAX_PRESSURE = 100
	DEFAULT_MIN_PRESSURE = 0
	DEFAULT_BUS          = 0
	DEFAULT_DEVICE       = "/dev/i2c-0"
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

// Initialize I2C Bus, this probably needs to be broken up better
// Need to make this so that the default bus and device can change
func (sensor *Pressure) initialize() {

	//	logger.ChangePackageLogLevel("i2c", logger.InfoLevel)
	//	i2cdevice, err := i2c.NewI2C(uint8(sensor.address), sensor.bus)

	i2cdevice, err := i2c.Open(&i2c.Devfs{Dev: "/dev/i2c-1"}, DEFAULT_ADDR)
	if err != nil {
		panic(err)
	}

	sensor.databus = *i2cdevice
}

/*
*************************************************
Function readCounts()
Descritpion: Reads the current pressure sensor data
register.
Returns: Presure Sensor Current ADC Counts
**************************************************
*/
func (sensor *Pressure) read_counts() int {

	//	dataSample, err := sensor.databus.ReadRegU16BE(DATA_REG)
	var buffer []byte
	buffer = append(buffer, 2)
	dataBuffer := sensor.databus.Read(buffer)
	dataSample := bytes.Buffer(dataBuffer)
	return int(dataSample)
}

/*
*************************************************
Function read_mbar()
Descritpion: Reads the current pressure sensor data
and converts it to milibars
Returns: Presure Sensor Pressure in Milibars (Float32)
**************************************************
*/
func (sensor *Pressure) read_mbar() float32 {

	return (sensor.counts2mbar(sensor.read_counts()))

}

/*
*************************************************
Function read_mbar()
Descritpion: Reads the current pressure sensor data
and converts it to milibars
Returns: Presure Sensor Pressure in Milibars (Float32)
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
	fmt.Println("Sensor Bus : ", sensor1.bus)
	var count = 0
	for i := 0; i < 100; i++ {

		dataSample := sensor1.read_counts()
		fmt.Println("mmHg Data Read :", strconv.FormatInt(int64(dataSample), 16))
		time.Sleep(100 * time.Millisecond)
		if dataSample > 2000.0 {
			count++
		}
	}
	println("error count: ", count)
	sensor1.databus.Close()
}
