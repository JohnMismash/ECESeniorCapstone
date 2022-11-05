from alarm import pp_alarm
from sounds import pp_sounds

import time

alarm_system = pp_alarm()
sounds = pp_sounds()

alarm_system.enable_alarm()
sounds.play_init_sounds()

while 1:
    
    if(alarm_system.is_alarm_triggered() == True):
        sounds.play_alarm_sounds()
        break 
        

time.sleep(5) 

print("Going to Disable Alarm")

alarm_system.disable_alarm()
sounds.stop_sounds()

print("Disable Alarm" , alarm_system.is_alarm_triggered())