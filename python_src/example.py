#!/usr/bin/python3

import time
import os
import pressure as sensor

POST_READ_DELAY = 0.1
WRITE_DELAY = 0.05

DATA_FILE = 'pressure_data_output'

def main():
    sensor1 = sensor.Pressure()
    display_info(sensor1)
    read_data(sensor1)
#-----------------------------------------------------------
# Function display_info()
# Displays the information about the sensor object to the 
# standard output for debugging.
#-----------------------------------------------------------
def display_info(sensor):

    print("Sensor Bus :" + str(sensor.bus))
    print("Sensor Bus :" + str(sensor.address))
    print("Sensor Min Counts :" + str(sensor.minCounts))
    print("Sensor Max Counts :" + str(sensor.maxCounts))
    print("Sensor Max Pressure :" + str(sensor.pressureMax))
    print("Sensor Min Pressure :" + str(sensor.pressureMin))

    time.sleep(2)       # 2 Second Display for Splash Screen
    os.system("clear")  # Clear Screen and Move On

def read_data(sensor):
         
    for i in range (0, 240 ):
        data=sensor.readSample()
        print(data)
        time.sleep(.1)
        pressure_m = sensor.counts2mBar(data)
        pressure_hg = sensor.mBar2mmhg(pressure_m)
        os.system("clear")
        print("pressure in mmhg is:" + str(pressure_hg)	+" pressure in mbar is:" + str(pressure_m))	
        time.sleep(WRITE_DELAY)



main()