#!/usr/bin/python3

import time
import os
import pressure as sensor

def main():
    sensor1 = sensor.Pressure()
    display_info(sensor1)
    read_data(sensor1)


def display_info(sensor):

    print("Sensor Bus :" + str(sensor.bus))
    print("Sensor Bus :" + str(sensor.address))
    print("Sensor Min Counts :" + str(sensor.minCounts))
    print("Sensor Max Counts :" + str(sensor.maxCounts))
    print("Sensor Max Pressure :" + str(sensor.pressureMax))
    print("Sensor Min Pressure :" + str(sensor.pressureMin))


def read_data(sensor):
         
    for i in range (0, 240 ):
        data=sensor.readSample()
        time.sleep(.1)
        pressure_m = sensor.counts2mBar(data)
        pressure_hg = sensor.mBar2mmhg(pressure_m)
        os.system("clear")
        print("pressure in mmhg is:" + str(pressure_hg)	+" pressure in mbar is:" + str(pressure_m))	
        time.sleep(.05)

main()