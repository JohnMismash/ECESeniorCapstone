import pygame
import time 

class pp_sounds:
    
    def __init__(self):
        
        pygame.mixer.init()

    """ Play Init Sounds  
    """
    def play_init_sounds(self):
        
        try:
            self.sound.stop()
        except:
            pass
        
        self.sound = pygame.mixer.Sound("/home/username/porchPirates/ECESeniorCapstone/Raspberry_Pi/sounds/beep.wav")
        self.sound.set_volume(1)
        
        for i in range(1,6):
        
            self.sound.play()
            time.sleep(0.5)
    
    """ Play Init Sounds  
    """
    def play_alarm_sounds(self):
        
        try:
            self.sound.stop()
        except:
            pass
        
        self.sound = pygame.mixer.Sound("/home/username/porchPirates/ECESeniorCapstone/Raspberry_Pi/sounds/alarm.wav")
        self.sound.set_volume(1)
        self.sound.play(loops = -1)
        
    
    """ Stop All Sounds 
    """
    def stop_sounds(self):
        
        try:
            self.sound.stop()
        except:
            pass
        
    