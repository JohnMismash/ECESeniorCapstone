
import time
import numpy as np
from smbus import SMBus
import pygame
import threading

bus = SMBus(1) # Create a new I2C-1 bus
addr = 0x6A

while 1:
    
    bus.write_byte_data(addr, 0x11, 0x60); # Set Accelerometer to High Performance Mode:
    x_low = bus.read_byte_data(addr, 0x22);
    x_high = bus.read_byte_data(addr, 0x23);
    x_val = np.int16(((x_high << 8) |  x_low))

    time.sleep(1)      
    print("X Value: ", hex(x_val), " = ", abs(x_val))
 
                