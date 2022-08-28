import RPi.GPIO as GPIO
import time 
GPIO.setmode(GPIO.BOARD)

# Using PIN 37 as OUTPUT PIN
GPIO.setup(37, GPIO.OUT)

# Toggle GPIO pin between turning on and off
# GPIO.output(37, True)
# GPIO.output(37, False)

# Create PWM signal with (GPIO Pin Number, frequency)  
myPWM = GPIO.PWM(37, 100)

# Start the PWM with Percent of Duty Cycle 0 - 100
myPWM.start(50)

print("First Program with PWM Signal. Also Check to see if function call is non blocking")
time.sleep(5) # Sleep for 5 Seconds 

# To Stop the PWM Signal 
myPWM.stop()

# To clean up pins for next use
GPIO.cleanup()