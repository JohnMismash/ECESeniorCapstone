import time
import RPi.GPIO as GPIO

class pp_door:
    global __isDoorOpen__
    global __isRFIDActive__

    """ Constructor for the door. 
    """
    def __init__(self):
        global __isDoorOpen__
        global __isRFIDActive__

        __isDoorOpen__ = False
        __isRFIDActive__ = False

        self.RFID_Pin = 40
        self.Magnet_Sensor_Pin = 38
        self.PWM_Pin = 37
        self.duty_cycle = 0

        self.__enable_GPIO__()

    def __enable_GPIO__(self):
        GPIO.setmode(GPIO.BOARD) # Pin Number Being Physical Board

        GPIO.setup(self.RFID_Pin, GPIO.IN, pull_up_down = GPIO.PUD_DOWN) # Set Pin with Pull Up Resistor and see 3.3V
        GPIO.setup(self.Magnet_Sensor_Pin, GPIO.IN, pull_up_down = GPIO.PUD_UP) # Set Pin with Pull Up Resistor and see 3.3V
        GPIO.setup(self.PWM_Pin, GPIO.OUT)
        
        self.myPWM = GPIO.PWM(self.PWM_Pin, 50)  # Create PWM signal with (GPIO Pin Number, frequency)  
        self.myPWM.start(self.duty_cycle)

    def openDoor(self):
        global __isDoorOpen__

        self.startTime = time.perf_counter()

        self.duty_cycle = 2.5
        self.myPWM.ChangeDutyCycle(self.duty_cycle)

        time.sleep(0.3) # Must Delay for timing

        self.duty_cycle = 0
        self.myPWM.ChangeDutyCycle(self.duty_cycle)

        __isDoorOpen__ = True

    def closeDoor(self):
        global __isDoorOpen__

        self.duty_cycle = 12.5
        self.myPWM.ChangeDutyCycle(self.duty_cycle)

        time.sleep(0.3) # Must Delay for timing

        self.duty_cycle = 0
        self.myPWM.ChangeDutyCycle(self.duty_cycle)

        __isDoorOpen__ = False

        self.startTime = None

    def isDoorOpen(self):
        global __isDoorOpen__

        checkDoor = GPIO.input(self.Magnet_Sensor_Pin)

        if (checkDoor != 1):
            __isDoorOpen__ = True
        
        else:
            __isDoorOpen__ = False

        return __isDoorOpen__
    

    def isRFIDActivated(self):
        global __isRFIDActive__

        if(GPIO.input(self.RFID_Pin) == 1):
            __isRFIDActive__ = True
        
        else:
            __isRFIDActive__ = False

        return __isRFIDActive__


    def getTimeElapsed(self):
        if self.startTime is None:
            print("Start Time is not initialized!")
            return 0

        elapsedTime = time.perf_counter() - self.startTime
        
        return elapsedTime        

   


