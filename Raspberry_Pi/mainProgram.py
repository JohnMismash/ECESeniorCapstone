#from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
#from /home/username/.local/lib/python3.9/site-packages/AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient  
# import importlib.util
# import sys
# spec = importlib.util.spec_from_file_location("AWSIoTPythonSDK.MQTTLib", "/home/username/.local/lib/python3.9/site-packages/AWSIoTPythonSDK/MQTTLib.py")
# AWSIoTMQTTClient = importlib.util.module_from_spec(spec)
# sys.modules["AWSIoTPythonSDK.MQTTLib"] = AWSIoTMQTTClient
# spec.loader.exec_module(AWSIoTMQTTClient)
#from .home.username.porchPirates.ECESeniorCapstone.Raspberry_Pi.alarm.py import pp_alarm
# /home/username/porchPirates/ECESeniorCapstone/Raspberry_Pi/alarm.py

#import sys
#sys.path.insert(1, '/home/username/.local/lib/python3.9/site-packages/AWSIoTPythonSDK.MQTTLib')
#import AWSIoTMQTTClient

import sys

print("Starting Import")
sys.path.insert(1, '/home/username/.local/lib/python3.9/site-packages/AWSIoTPythonSDK')
from MQTTLib import AWSIoTMQTTClient as AWSIoTMQTTClient

#from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient

import time
import RPi.GPIO as GPIO

from alarm import pp_alarm
from door import pp_door

from sounds import pp_sounds

global alarmCurrentlyTriggered
global doorIsInClosedSleepState

BEAM_PIN = 11

def break_beam_callback(channel):
    if GPIO.input(BEAM_PIN):
        print("beam unbroken")
    else:
        print("beam broken")
        # We got a package
        packageArrival()
        

def openDoor_callback(self, params, packet):
    print('Message Received: Open Door')
    print('Topic: ' + packet.topic)
    print('Payload: ', (packet.payload))

    global doorIsInClosedSleepState
    if not door.isDoorOpen():
        alarm_system.disable_alarm()
        door.openLatch()
        time.sleep(4)
        doorIsInClosedSleepState = False    
        
        


def turnOffAlarm_callback(self, params, packet):
    print('Message Received: TURN OFF ALARM')
    print('Topic: ' + packet.topic)
    print('Payload: ', (packet.payload))
    
    sounds.stop_sounds()
    
    global alarmCurrentlyTriggered
    if alarm_system.is_alarm_triggered():
        alarm_system.disable_alarm()
        alarm_system.enable_alarm()
        sounds.play_init_sounds()
    alarmCurrentlyTriggered = False
    
        


def packageArrival():
    print('publishing package arrival')
    myMQTTClient.publish(
        topic = "home/packageArrival", 
        QoS = 1,
        payload = "{\n\"Message\": \"Package Has Arrived\"\n}"
    )


def theftDetection():
    print('publishing theft detection')
    myMQTTClient.publish(
        topic = "home/theftDetection", 
        QoS = 1,
        payload = "{\n\"Message\": \"A theif has been detected\"\n}"
    )


# Main
# For certificate based connection
myMQTTClient = AWSIoTMQTTClient("TONYclientid")#

# For TLS mutual authentication
myMQTTClient.configureEndpoint("a2mlci86nk2fzg-ats.iot.us-west-1.amazonaws.com", 8883) #Provide your AWS IoT Core endpoint (Example: "abcdef12345-ats.iot.us-east-1.amazonaws.com")
myMQTTClient.configureCredentials("/home/username/certs/Amazon-root-CA-1.pem", "/home/username/certs/private.pem.key", "/home/username/certs/device.pem.crt") #Set path for Root CA and unique device credentials (use the private key and certificate retrieved from the logs in Step 1)

myMQTTClient.configureOfflinePublishQueueing(-1) #infinite offline Publish queueing
myMQTTClient.configureDrainingFrequency(2) # Draining: 2Hz
myMQTTClient.configureConnectDisconnectTimeout(10) # 10 sec
myMQTTClient.configureMQTTOperationTimeout(5) # 5 sec

print ('Initiating IoT Core Topic ... ')
myMQTTClient.connect()
print ('Success!')

myMQTTClient.subscribe("home/openDoor", 1, openDoor_callback)
myMQTTClient.subscribe("home/turnOffAlarm", 1, turnOffAlarm_callback)


# break beam set up
GPIO.setmode(GPIO.BOARD) # Pin Number Being Physical Board
GPIO.setup(BEAM_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.add_event_detect(BEAM_PIN, GPIO.BOTH, callback=break_beam_callback)


sounds = pp_sounds()

# Enable the alarm
alarm_system = pp_alarm()
alarm_system.enable_alarm()

sounds.play_init_sounds()

door = pp_door()

alarmCurrentlyTriggered = False
rfidCurrentlyTriggered = False
doorIsInClosedSleepState = True

while True:
    # print(door.isDoorOpen())
    # this acts like a positive edge trigger for the alarm being triggered. So it will only trigger once
    if (not alarmCurrentlyTriggered) and alarm_system.is_alarm_triggered():
        alarmCurrentlyTriggered = True
        theftDetection()
        sounds.play_alarm_sounds()
        
        
    
    # if the user puts rfid on a closed door, open the latch
    if (not rfidCurrentlyTriggered) and (door.isRFIDActivated() and (not door.isDoorOpen())):
        door.openLatch()  # unlock the door
        alarm_system.disable_alarm()  # turn off the alarm
        
        rfidCurrentlyTriggered = True   
        doorIsInClosedSleepState = False
        time.sleep(4)
        
        
    # if the door is fully closed and noRFID, time to lock the door
    if (not doorIsInClosedSleepState) and (not door.isDoorOpen()) and (not door.isRFIDActivated()):
        print("close door")
        time.sleep(4) # we sleep here to let the door be fully closed
        door.closeLatch()
        rfidCurrentlyTriggered = False
        doorIsInClosedSleepState = True
        alarm_system.enable_alarm()
        
        
    
            
    #if (not door.isRFIDActivated() and door.isDoorOpen()):
        # elapsedTime = door.getElapsedTime()
        # if (elapsedTime > 30):
            # if (!door.isDoorOpen()):
                # LOCK THE DOOR
                # door.closeDoor()
            # else:
                # PLAY ALARM SOUND!


