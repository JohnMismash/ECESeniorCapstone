# ECE5780_Project
ECE 5780 Final Project: 

Team Members:
    Bailey Martin
    Nathan Sartnurak

Project Name: Alarm System 
Source Code: \ECE5780_Project\Final_Project\Core\main.c 

Purpose: 

The purpose of this project is to apply what we learned in the class to create an embedded system. In our case, we created a motion detector alarm system. This system will be used to interface with the Porch Pirates senior project team which involves securing a package using a scissor lift mechanism.  Our portion for this project is to create a tampering detection alarm system that notifies the user if someone is attempting to steal the package. This project behaves similarly to a Xfinity home defender system where sensors are distributed around the house and an input code is typed to enable the alarm.  At this point, if an input sensor is triggered, an alarm is triggered by producing a loud enough sound to alert the user. Once the alarm is triggered, the only way to turn off the alarm is for the user to correctly type the 4-digit code.  Once the alarm has been disabled, the user can turn on the alarm by typing in the passcode again. 

Functionality: 

The functionality of this project can be broken down into four milestones: 

Milestone 1:
Use USB-USART cable to create an interface between STM32F0 and Putty Terminal. This will act as our user input for our embedded system.  Using this, allow the user to type a four-digit pin to arm and disarm the alarm. This will be done through an interrupt driven architecture that responds to every interrupt call using the USART interrupt handler. To test this interface between the STM23F0 and the putty terminal, turn LEDS on the STM on if the 4-digit password is correct, and turn off LEDS if the password is incorrect. 

Milestone 2: 
Create an interface between the accelerometer and the STM32F0 board. The acetometer chip that we are using is the ISM330DHCX (https://www.adafruit.com/product/4502) which features a high-performance 3d digital accelerometer and 3D digital gyroscope. To do this, read go through who am I protocol to see if communication between STM32F0 and accelerometer chip is possible. Then use the waveform generator and read the acceleration values from the accelerometer to STM32F0 board through the I2C protocol. To test milestone 2, turn LEDs on once the accelerometer record a certain acceleration value. 

Milestone 3: 
Create an interface between the STM32FO board and the speaker. To do this we will need to use an amplifier that takes in analog voltage signal. To generate an input signal, use pulse width modulation (PWM) at a certain frequency value.  For this amplifier to produce sound, the PWM must be ran at an 800 Hz signal and is bounded between 0 and 3V. Configure the PWM to produce this signal and test this by using an oscilloscope and measure the frequency of the generated signal.  To test this, use a GPIO pin that connects an amplifier, which will be connected to a speaker. Generated the PWM signal and check to see if sound is being produced from the speaker. 

Milestone 4: 
Interface milestone 1, milestone 2, and milestone 3 together to create an alarm system. Create protocol to enable alarm system and create protocol to disable alarm system.  To enable the alarm, the protocol must allow the user to type in a 4-digit password. Once the alarm is enabled, an alarm should be triggered if the accelerometer detects a high enough acceleration value. Once the alarm is triggered, a very loud sound should be produced of the speaker every 1 second. To disable the alarm, the user must type in the same 4-digit password the user typed earlier to enable the alarm. If the user incorrectly types the 4-digit password, do not disable alarm, and keep the alarm ringing. 

Basic Instructions:

Use the block diagram (block_diagram.png) to create alarm system. Once the system is built, open a putty terminal using a serial port with a baud rate of 115200. Then click the reset button on the STM32F0 board to reset the system. This should prompt you to enter a 4-digit pass code to enable the alarm. Once you decided on a passcode to enable the alarm, type it into the terminal and the speaker should produce a loud sound for at 5 seconds. Check the BCD to 7segment display as it counts down from 5 all the way to 0. At this point, the alarm is enabled and waiting for a sensor to trigger an alarm. To do this move the accelerometer and the alarm should be triggered. Once this happens, the speaker should be producing aloud sound every 1 second.  The BCD to 7 segment display should also be displaying a number every second. To turn off the alarm, type in the same four-digit pass that you typed in to enable the alarm. If you do not type the correct four-digit password the alarm will continue to stay on. Once the four-digit password is correct, the alarm should be disabled, meaning that the speaker should produce no sound and the BCD to 7 segments be turned off. After this, a prompt on the putty terminal should indicate the alarm has been turned off and ask for a four-digit password again to enable the alarm. If the user wants to enable the alarm, type in the any four-digit passcode again and the protocol above will repeat itself. 