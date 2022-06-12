# Play Sounds with pygame

import pygame
import time 

path = "/home/pi/ECESeniorCapstone/Alarm/Learning/"

sound_files = ["alarm.wav"]

# Pygame Setup
pygame.mixer.init()
speaker_volume = 0.5 # 50% Volume

pygame.mixer.music.set_volume(speaker_volume)
pygame.mixer.music.load(path + sound_files[0])
pygame.mixer.music.play()

while 1:
    pygame.mixer.music.play()
    
    while pygame.mixer.music.get_busy() == True:
        continue
    
    #time.sleep(0.5)
    
    

