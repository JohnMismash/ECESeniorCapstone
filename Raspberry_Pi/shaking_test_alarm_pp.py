from pp_alarm import pp_alarm

import time

alarm_system = pp_alarm()
alarm_system.enable_alarm()

time.sleep(5)

print("Going to Disable Alarm")
alarm_system.disable_alarm()
print("Disable Alarm")