#!/usr/bin/python3

import time
from datetime import datetime
import os
import pressure as sensor

POST_READ_DELAY = 0.1   # This is needed with the smbus python library to prevent the unit from reading the i2c bus too quickly. 
WRITE_DELAY = 0.05      # Update Delay, found this made things smoother.

DATA_FILE = 'pressure_data_output'

#-----------------------------------------------------------
# Function Main()
# Main Routine, Create a sensor object, display its atributes,
# Take Some Data, Write to a File.
#-----------------------------------------------------------

def main():
    sensor1 = sensor.Pressure()
    display_info(sensor1)
    acqData = read_data(sensor1)
    print(acqData)
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
    dataList = [] # empty list to hold data         
    for i in range (0, 240 ):
        counts=sensor.readCounts()
        timeStamp = datetime.now()                  # Grab a Timestamp
        time.sleep(POST_READ_DELAY)                 # Wait 
        pressureMbar = round(sensor.counts2mBar(counts),3)   # Convert Raw Counts to mBar
        pressureMmhg = round(sensor.mBar2mmhg(pressureMbar),3) # Convert mBar pressure to mmHg
        
        os.system("clear")
        print("-----------------------------------")
        print("pressure in mmhg is : " + str(counts))
        print("-----------------------------------")
        print("pressure in mmhg is : " + str(pressureMmhg))
        print("-----------------------------------")
        print("pressure in mbar is : " + str(pressureMbar))	
        print("-----------------------------------")
        time.sleep(WRITE_DELAY)
        dataList.append([i,timeStamp, counts, pressureMbar, pressureMmhg])     
    return dataList



main()