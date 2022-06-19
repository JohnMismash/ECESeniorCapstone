
import time
import numpy as np
from smbus import SMBus
import pygame
import threading

"""Thread class with a stop() method. The thread itself has to check
    regularly for the stopped() condition.
"""
class MyThread(threading.Thread):
   
    def __init__(self, *args, **kwargs):
        super(MyThread, self).__init__(*args, **kwargs)
        self._stop = threading.Event()

    def stop(self):
        self._stop.set()

    def stopped(self):
        return self._stop.isSet()

class pp_alarm:

    global alarm_triggered
    alarm_triggered = False 
    
    """ Constructor for the alarm system. Test to see if the i2c bus can be reached. 
    """
    def __init__(self):
        
        self.bus = SMBus(1) # Create a new I2C-1 bus
        self.addr = 0x6A
        

        print("Starting I2C Protocal Communication with Acceloerometer")
        
        try:
            val = self.bus.read_byte_data(0x6A, 0x0F); # Who AM I Protocal
        except:
            print("Something Wrong with I2C bus on", hex(self.addr), "\nCheck bus using: i2cdetect -y 1")
            return


    """ Enabling the Alarm: require shaking the box to trigger alarm 
    """
    def enable_alarm(self):
        
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

        # Multithreading Alarm
        self.practicethread = MyThread(target=self.__theft_detection__)
        self.practicethread.start()
                

    def __theft_detection__(self):

        global alarm_triggered

        while not self.practicethread.stopped():
            self.bus.write_byte_data(self.addr, 0x11, 0x60); # Set Accelerometer to High Performance Mode:

            x_low = self.bus.read_byte_data(self.addr, 0x22);
            x_high = self.bus.read_byte_data(self.addr, 0x23);
            x_val = np.int16(((x_high << 8) |  x_low))
            
            # print("X Value: ", hex(x_val), " = ", abs(x_val))
    
            if(abs(x_val) > 4000 and (alarm_triggered == False)):
                
                print("Alarm Triggered")
                
                # Pygame Setup
                self.sound = pygame.mixer.Sound("./sounds/alarm.wav")
                self.sound.set_volume(0.5)
                self.sound.play(loops = -1)
                alarm_triggered = True 
                return 
                
     
    def disable_alarm(self):
        global alarm_triggered
        
        self.practicethread.stop()
        alarm_triggered = False 
        try:
            self.sound.stop()
        except:
            ...

    def is_alarm_triggered(self):
        return  alarm_triggered