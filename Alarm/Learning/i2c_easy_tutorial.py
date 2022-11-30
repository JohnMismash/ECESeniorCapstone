from smbus import SMBus
import time
import numpy as np

bus = SMBus(1) # Create a new I2C-1 bus
addr = 0x6A

print("Starting I2C Protocal")
val = bus.read_byte_data(addr, 0x0F); # Who AM I Protocal
print("Who am I:", hex(val))
bus.write_byte_data(addr, 0x11, 0x60); # High Performance Mode:

while 1:
    
    #bus.write_byte_data(addr, 0x11, 0x60); # High Performance Mode:
    #x_low = bus.read_byte_data(addr, 0x22);
    #x_high = bus.read_byte_data(addr, 0x23);
    #x_value = ((x_high << 8) |  x_low)
    #x_val = np.int16(x_value)
    #print("X Value: ", hex(x_value), " = ", abs(x_val))

    #bus.write_byte_data(addr, 0x11, 0x60); # High Performance Mode:
    #y_low = bus.read_byte_data(addr, 0x24);
    #y_high = bus.read_byte_data(addr, 0x25);
    #y_value = ((y_high << 8) |  y_low)
    #y_val = np.int16(y_value)
    #print("Y Value: ", hex(y_value), " = ", abs(y_val))

    bus.write_byte_data(addr, 0x11, 0x60); # High Performance Mode:
    z_low = bus.read_byte_data(addr, 0x26);
    z_high = bus.read_byte_data(addr, 0x27);
    z_value = ((z_high << 8) |  z_low)
    z_val = np.int16(z_value)
    print("Z Value: ", hex(z_value), " = ", abs(z_val))

    time.sleep(1)
        


    
