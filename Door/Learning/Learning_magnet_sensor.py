from time import sleep
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BOARD) # Pin Number Being Physical Board

button1 = 32; # PIN 16 

GPIO.setup(button1, GPIO.IN, pull_up_down = GPIO.PUD_UP) # Set Pin with Pull Up Resistor and see 3.3V  
# GPIO.setup(button2, GPIO.IN) # Set Pin with Pull Up Resistor and see 3.3V  

# Door Closed is Low
# Door Open is High 

while(1):
    
    if(GPIO.input(button1) == 1):
        print("Read High Voltage")
        sleep(.1)
    else:
        print("Read Low Voltage")
        sleep(.1)
        
        
