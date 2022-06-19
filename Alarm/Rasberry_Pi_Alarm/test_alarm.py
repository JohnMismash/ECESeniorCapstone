from alarm import pp_alarm
import time

alarm_system = pp_alarm()
alarm_system.enable_alarm()

time.sleep(10)

print(alarm_system.is_alarm_triggered())

print("Going to Disable Alarm")
alarm_system.disable_alarm()
print("Disable Alarm" , alarm_system.is_alarm_triggered())