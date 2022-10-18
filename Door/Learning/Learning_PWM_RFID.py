import time 
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD) # Pin Number Being Physical Board


# Input Voltage Pin
Input_Voltage_Pin = 40; 
GPIO.setup(Input_Voltage_Pin, GPIO.IN, pull_up_down = GPIO.PUD_DOWN) # Set Pin with Pull Up Resistor and see 3.3V  

# PWM at Pin  37
GPIO.setup(37, GPIO.OUT)

# Create PWM signal with (GPIO Pin Number, frequency)  
myPWM = GPIO.PWM(37, 50)
myPWM.start(0)

duty_cycle = 0  

while(1):
    
    if(GPIO.input(Input_Voltage_Pin) == 1):
        
        if(duty_cycle != 2.5):
            
            print("Open Door")
            
            duty_cycle = 2.5
            myPWM.ChangeDutyCycle(duty_cycle)
            time.sleep(0.3   ) # Must Delay for timing 
            myPWM.ChangeDutyCycle(0)
        
    else:
        
        if(duty_cycle != 12.5):
            
            print("Close Door")
            
            duty_cycle = 12.5
            myPWM.ChangeDutyCycle(duty_cycle)
            time.sleep(0.3) # Must Delay for timing
            myPWM.ChangeDutyCycle(0)
        
        
        
        
