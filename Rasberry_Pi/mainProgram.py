import time
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import RPi.GPIO as GPIO
from alarm import pp_alarm

BEAM_PIN = 17

def break_beam_callback(channel):
    if GPIO.input(BEAM_PIN):
        print("beam unbroken")
    else:
        print("beam broken")

def openDoor_callback(self, params, packet):
    print('Message Received: Open Door')
    print('Topic: ' + packet.topic)
    print('Payload: ', (packet.payload))


def turnOffAlarm_callback(self, params, packet):
    print('Message Received: TURN OFF ALARM')
    print('Topic: ' + packet.topic)
    print('Payload: ', (packet.payload))
    if alarm_system.is_alarm_triggered():
        alarm_system.disable_alarm()
        alarm_system.enable_alarm()


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
GPIO.setmode(GPIO.BCM)
GPIO.setup(BEAM_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.add_event_detect(BEAM_PIN, GPIO.BOTH, callback=break_beam_callback)

# Enable the alarm
alarm_system = pp_alarm()
alarm_system.enable_alarm()



while True:
    packageArrival()
    time.sleep(15)
    theftDetection()
    time.sleep(15)

