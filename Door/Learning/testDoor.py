import time
from door import pp_door

door = pp_door()

while (1):
    if(door.isDoorOpen() == False):
        print("DOOR CLOSED")
    else:
        print("DOOR OPEN")
        break

door.openDoor()

print(door.isDoorOpen())
