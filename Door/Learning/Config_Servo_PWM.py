import RPi.GPIO as GPIO
import time 
GPIO.setmode(GPIO.BOARD)

# Using PIN 37 as OUTPUT PIN
GPIO.setup(37, GPIO.OUT)

# Toggle GPIO pin between turning on and off
# GPIO.output(37, True)
# GPIO.output(37, False)

# Create PWM signal with (GPIO Pin Number, frequency)  
myPWM = GPIO.PWM(37, 50)

# Start the PWM with Percent of Duty Cycle 0 - 100

myPWM.start(7.5)
myPWM.ChangeDutyCycle(7.5)


"""
try:
    while 1:
        myPWM.ChangeDutyCycle(7.5)
        print("Duty Cycle is 7.5")
        time.sleep(3)
        myPWM.ChangeDutyCycle(2.5)
        print("Duty Cycle is 2.5")
        time.sleep(3)
        myPWM.ChangeDutyCycle(12.5)
        print("Duty Cycle is 12.5")
        time.sleep(3)

except KeyboardInterrupt:
    myPWM.stop()
    GPIO.cleanup()
    
"""    
        

#time.sleep(5) # Sleep for 5 Seconds 

# To Stop the PWM Signal 
#myPWM.stop()

# To clean up pins for next use
#GPIO.cleanup()
