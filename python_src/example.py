#!/usr/bin/python3
#-------------------------------------------------------
# This is a Test Program demonstating reading from a
# Honeywell ABPD sensor using the pressure class included
# With This Example.  This program displays the sensor
# Information, Aquires Data, and Writes the Data to a 
# CSV.
#
# This program leverages the smbus i2c library
#-------------------------------------------------------


# import Statements
import time
import datetime
import os
import csv
import pressure as sensor

#Constants
POST_READ_DELAY = 0.1   # This is needed with the smbus python library to prevent the unit from reading the i2c bus too quickly. 
WRITE_DELAY = 0.05      # Update Delay, found this made things smoother.
DEFAULT_NUM_SAMPLES=240 # Default Number of Data Samples to Aquire

DATA_FILE = 'pressure_data.csv'  # File to write out.

#-----------------------------------------------------------
# Function Main()
# Main Routine, Create a sensor object, display its atributes,
# Take Some Data, Write to a File.
#-----------------------------------------------------------

def main():
    sensor1 = sensor.Pressure()                             # Create a Sensor Object
    display_info(sensor1)                                   # Display the Sensors Info
    acqData = read_data(sensor1,DEFAULT_NUM_SAMPLES)        # Aquire some Sample Data
    writeData(acqData,DATA_FILE)                            # Write Data to a CSV File
    exit(0)                                                 # Clean Exit

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

#-----------------------------------------------------------
# Function read_data()
# Displays the information about the sensor object to the 
# standard output for debugging.
# Args - sensor - A Sensor Object.
# Returns - List of Data and Timestamps
#-----------------------------------------------------------

def read_data(sensor,numSamples):
    
    dataList = ['SampleId','Time Stamp','ADC Counts','Pressure(mBar)','Pressure(mmHg)'] # list to hold data, Made top row headers

    for i in range (0, numSamples ):                                   # Sample the Transducer 240x
    
        counts=sensor.readCounts()
        timeStamp = str(datetime.datetime.now())                     # Grab a Timestamp
        time.sleep(POST_READ_DELAY)                                 # Wait for i2c bus 
        pressureMbar = round(sensor.counts2mBar(counts),3)          # Convert Raw Counts to mBar
        pressureMmhg = round(sensor.mBar2mmhg(pressureMbar),3)      # Convert mBar pressure to mmHg
        
        os.system("clear")                                      #Clear Screen between Samples
        print("-----------------------------------")            # Print some Info to Standard Output
        print("Sensor ADC Counts is: " + str(counts))
        print("-----------------------------------")
        print("pressure in mmhg is : " + str(pressureMmhg))
        print("-----------------------------------")
        print("pressure in mbar is : " + str(pressureMbar))	
        print("-----------------------------------")
        print(timeStamp)
        time.sleep(WRITE_DELAY)                                # Wait after Sample Read.

        dataList.append([i,timeStamp, counts, pressureMbar, pressureMmhg])     # Write data to List
    
    return dataList

#-----------------------------------------------------------
# Function write_data()
# Writes data to the CSV filename passed to the function.
# Args - data - Data List
#      - fileName - name of Data File to Write
#-----------------------------------------------------------
def writeData(data,fileName):

    with open(fileName, 'w') as f: # Open Data File
        write = csv.writer(f)       # Create csv Writer object
        write.writerows(data)       # Write Data list to CSV

main()