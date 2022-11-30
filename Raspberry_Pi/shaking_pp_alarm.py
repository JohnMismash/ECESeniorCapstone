
import time
import numpy as np
from smbus import SMBus
import pygame

class pp_alarm:
    
    """ Constructor for the alarm system. Test to see if the i2c bus can be reached. 
    """
    def __init__(self):
        
        self.bus = SMBus(1) # Create a new I2C-1 bus
        self.addr = 0x6A
        
        print("Starting I2C Protocal Communication with Acceloerometer")
        
        try:
            val = self.bus.read_byte_data(0x6A, 0x0F); # Who AM I Protocal
            # print("Who am I:", hex(val))
            
        except:
            print("Something Wrong with I2C bus on", hex(self.addr), "\nCheck bus using: i2cdetect -y 1")
            return
        

        # Down Count by 5 seconds
        pygame.mixer.init()
        pygame.mixer.music.set_volume(1)
        pygame.mixer.music.load("./sounds/beep.wav")
        pygame.mixer.music.play()
        
        
        for i in range(1,6):
            pygame.mixer.music.play()
    
            # Wait Until first sound is done
            while pygame.mixer.music.get_busy() == True:
                continue
            
            time.sleep(0.5) 
        
        
        print("Alarm Initialized")
        


    """ Enabling the Alarm: require shaking the box to trigger alarm 
    """
    def enable_alarm(self):

        shaking_counter = 0
        timer_start = False
        start_time = 0
        end_time = 0
        elapsed_time = 0
        prev_counter = 0
        
        while 1:
            
            self.bus.write_byte_data(self.addr, 0x11, 0x60); # High Performance Mode:

            x_low = self.bus.read_byte_data(self.addr, 0x22);
            x_high = self.bus.read_byte_data(self.addr, 0x23);
            x_val = np.int16(((x_high << 8) |  x_low))
            
            # print("X Value: ", hex(x_value), " = ", abs(x_val))
    
            time.sleep(1)
    
            if(abs(x_val) > 4000):
                
                print("X Value: ", hex(x_val), " = ", abs(x_val))
                shaking_counter += 1
                
                # Start Timer
                if(timer_start == False):
                    
                    start_time = time.time()
                    timer_start = True
                
            
            if(timer_start == True):
                
                end_time = time.time()
                elapsed_time = end_time - start_time
                
                # print("Elapsed Time:", elapsed_time)
                # print("Prev Counter:", prev_counter, "Shake Counter", shaking_counter)
                
                if(elapsed_time > 5 and prev_counter == shaking_counter):
                    
                    print("Alarm Reset")
                    timer_start = False
                    shaking_counter = 0
            
            
            # Triggering Alarm 
            if(shaking_counter > 4):
                print("Alarm Triggered")
                
                # Pygame Setup
                self.sound = pygame.mixer.Sound("./sounds/alarm.wav")
                self.sound.set_volume(0.5)
                self.sound.play(loops = -1)

                # 1. Make thread and run/ Make process Concurrency 
                #    Or do Asynchronous through asyncIO on context switching  

                # Pygame Setup
                # pygame.mixer.init()
                # speaker_volume = 0.5 # 50% Volume
        
                # pygame.mixer.music.set_volume(speaker_volume)
                # pygame.mixer.music.load( self.path + self.sound_files[1])
                

                # while 1:
                    
                #     print("Producing Sounds")

                #     pygame.mixer.music.play()
    
                #     # Wait Until first sound is done
                #     while pygame.mixer.music.get_busy() == True:
                #         continue
            
                #     time.sleep(0.5)
                    
                #     # if flag is true stop producing sound 

                return 

            prev_counter = shaking_counter
    

    def disable_alarm(self):
        self.sound.stop()
