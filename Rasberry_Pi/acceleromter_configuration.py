
import time
import numpy as np
from smbus import SMBus
import pygame
import threading

bus = SMBus(1) # Create a new I2C-1 bus
addr = 0x6A

bus.write_byte_data(addr, 0x11, 0x60); # Set Accelerometer to High Performance Mode:

while 1:

    print("Reading X, Y, Z Values")

    # X Values:  
    x_low = bus.read_byte_data(addr, 0x22);
    x_high = bus.read_byte_data(addr, 0x23);
    x_val = np.int16(((x_high << 8) |  x_low))
    print("X Value: ", x_val)
    #print("X Value: ", hex(x_val), " = ", abs(x_val))

    # Y Values:
    y_low = bus.read_byte_data(addr, 0x24);
    y_high = bus.read_byte_data(addr, 0x25);
    y_val = np.int16(((y_high << 8) |  y_low))
    print("Y Value: ", y_val)
    # print("Y Value: ", hex(y_val), " = ", abs(y_val))

    # Z Values: 
    z_low = bus.read_byte_data(addr, 0x26);
    z_high = bus.read_byte_data(addr, 0x27);
    z_val = np.int16(((z_high << 8) |  z_low))
    print("Z Value: ", z_val)
    
    #print("Z Value: ", hex(z_val), " = ", abs(z_val))


    time.sleep(1)      
 
                