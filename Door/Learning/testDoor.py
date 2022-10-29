import time
from door import pp_door

door = pp_door()

""" Test for Elapsed Time 
time.sleep(1)
door.openDoor()
time.sleep(5)

print("Elapsed Time: " + str(door.getTimeElapsed()))
door.closeDoor()

print("Elapsed Time: " + str(door.getTimeElapsed()))
"""

""" Test for Magnetic Sensor
while (1):
    if(door.isDoorOpen() == False):
        print("DOOR CLOSED")
        time.sleep(1)
    else:
        print("DOOR OPEN")
        time.sleep(1)
        break

door.openDoor()

print(door.isDoorOpen())
"""

""" Test for RFID 
while (1):
    if(door.isRFIDActivated() == False):
        print("RFID OFF")
        time.sleep(1)
    else:
        print("RFID ON")
        time.sleep(1)
        break

door.openDoor()
print(door.isDoorOpen())
"""