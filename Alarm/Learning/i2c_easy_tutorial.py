from smbus import SMBus
import time
import numpy as np

bus = SMBus(1) # Create a new I2C-1 bus
addr = 0x6A

print("Starting I2C Protocal")
val = bus.read_byte_data(addr, 0x0F); # Who AM I Protocal
print("Who am I:", hex(val))


while 1:
    
    bus.write_byte_data(addr, 0x11, 0x60); # High Performance Mode:

    x_low = bus.read_byte_data(addr, 0x22);
    x_high = bus.read_byte_data(addr, 0x23);
    x_value = ((x_high << 8) |  x_low)
  
    x_val = np.int16(x_value)
    print("X Value: ", hex(x_value), " = ", abs(x_val))
    
    time.sleep(1)
    
    if(abs(x_val) > 4000):
        print("X Value: ", hex(x_value), " = ", abs(x_val))
        print("Trigged Done")
        exit()
        


    
