import time 
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD) # Pin Number Being Physical Board

# RFID Pin
RFID_Pin = 40; 
GPIO.setup(RFID_Pin, GPIO.IN, pull_up_down = GPIO.PUD_DOWN) # Set Pin with Pull Up Resistor and see 3.3V  

# Magnetic Sensor 
Magnet_Sensor_Pin = 32;  
GPIO.setup(Magnet_Sensor_Pin, GPIO.IN, pull_up_down = GPIO.PUD_UP) # Set Pin with Pull Up Resistor and see 3.3V  

# PWM
PWM_Pin = 37; 
GPIO.setup(PWM_Pin, GPIO.OUT)
myPWM = GPIO.PWM(PWM_Pin, 50)  # Create PWM signal with (GPIO Pin Number, frequency)  
myPWM.start(0)

duty_cycle = 0  

while(1):
    
    if(GPIO.input(RFID_Pin) == 1):
        
        if(duty_cycle != 2.5):
            
            print("Open Door")
            
            duty_cycle = 2.5
            myPWM.ChangeDutyCycle(duty_cycle)
            time.sleep(0.3) # Must Delay for timing 
            myPWM.ChangeDutyCycle(0)
            
            time.sleep(3) # Give User Time to Open Door
            
            check_door = GPIO.input(Magnet_Sensor_Pin)
            
            # While User Has Door Open do nothing
            if(check_door == 1):
                 while(check_door == 1):
                     check_door = GPIO.input(Magnet_Sensor_Pin)
            else:
                print("User Did not Open Door")
                    
            time.sleep(1.5) # Ensure Door is Close 
            
            print("Close Door")
            
            duty_cycle = 12.5
            myPWM.ChangeDutyCycle(duty_cycle)
            time.sleep(0.3) # Must Delay for timing
            myPWM.ChangeDutyCycle(0)
                

        
        
        

