from sounds import pp_sounds
import time 

sounds = pp_sounds()

sounds.stop_sounds()

sounds.play_init_sounds()

sounds.play_alarm_sounds()

time.sleep(1)

sounds.play_alarm_sounds()


time.sleep(3)

sounds.stop_sounds()

first_time = 0; 

while 1:
    first_time = first_time + 1; 
    if(first_time == 1):
        sounds.play_alarm_sounds()
  
    if(first_time == 5):
        sounds.stop_sounds()
        break 


