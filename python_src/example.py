#!/usr/bin/python3

import time
import os
import pressure.py as sensor

def main():
    
    print "the unit is on bus 0, Address"
    print hex(address)
    
    for i in range (0, 240 ):
	    data=readSample()
        time.sleep(.1)
        pressure_m = counts2mBar(data)
	    pressure_hg = mBar2mmhg(pressure_m)
	    os.system("clear")
	    print "pressure in mmhg is:" + str(pressure_hg)	+" pressure in mbar is:" + str(pressure_m)	
	    time.sleep(.05)

main()