package main

import (
	"fmt"
	"time"
)

func main() {

	// Declare a Sensor Object
	var sensor1 = Transducer.newSensor()
	fmt.Println("Sensor Bus : ", sensor1.bus)
	var count = 0
	for i := 0; i < 100; i++ {
		time.Sleep(100 * time.Millisecond)
		dataSample := sensor1.read_counts()
		fmt.Println("mmHg Data Read :", dataSample)
		time.Sleep(10 * time.Millisecond)
		if dataSample > 150.0 {
			count++
		}
	}
	println("error count: ", count)
	sensor1.databus.Close()
}
