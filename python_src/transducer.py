#!/usr/bin/python3

class transducer:
    
    #Define Device Status Register Value
    STATUS_REG = 0x00

    #Define Device Status Register Values
    NORMAL_OPERATION = 0x00
    COMMAND_MODE = 0x01
    STALE_DATA = 0x02
    DIAG_CONDITION = 0x03

    #Define Data Register Value
    DATA_REG = 0x00

    #Atributes
    countsmin = 0 #Max ADC Counts
    countsmax = 0 # Min ADC Counts
    pmin = 0      # Min Pressure (mBar)
    pmax = 0      # Max Pressure (mBar)
    address = 0   # I2C address of the Transducer IC
    bus = 0       # I2C Bus of the Transducer
    fd = 0	      # Transducer Format Descriptor


