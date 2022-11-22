# ECE5780 Lift Prototype
The 5780 lift prototype project is a precursor for the lift that will be constructed as part of the senior capstone project. The project is a mechanized scissor lift that raises and lowers via microcontroller commands.  A package is placed on top when the lift is fully extended and will trigger the lifting process. The goals that our team had described for our project are as follows:

Redesign PCB for a 12V motor - Previous homework assignments taught us how to design custom PCBs. The board designed for the homework did not deliver enough power to make our chosen motor work.

Have 2 STM boards that communcated via UART - One board managed the distance sensor and upper limit switch. The other board received messages, received the bottom limit sensor interrupt, and translated instructions to the motor.

Integrate Sensor Interrupts to trigger movement - The primary sensor used in this project was a distance sensor purchased from ADAFruit. If a package was placed on the lift, the sensor would put out a high signal.  Limit switches were used to determine the stopping height when the lift was extended or lowered.

Discharge the package from the lift when the lift lowered - Our original design included motorized rollers to discharge the package off of the top of the lift. Due to weight, spacing, and complexity concerns, we found roller ball transfer bearings that completed the task and avoided the potential problems we were having. 

### Folders in the Repo
In this repo we have the folders 5780, Motor Calculations, PCB, and Sensor Testing.

5780: Contains the microcontroller c code.  This folder has "Top_STM" and "Bottom_STM" subfolders that monitor sensor data and send instructions to the motor respectively

Motor Calculations: Contains the files created to determine parameters of the project. We used this math to help select a motor and design a PCB that could power the motor.

PCB: Contains the design files of the custom PCB.

Sensor Testing: Basic Arduino code and schematics that were used to determine how the sensors worked and how to integrate them in the final design.

### Block Diagram
![Our Block Diagram](https://github.com/JohnMismash/ECESeniorCapstone/blob/main/Motor%20Calculations/5780BlockDiagram%20-%20Page%201.png)

### Team Members:
 John 'Jack' Mismash - u1179865,
 Andrew Porter - u1071655,
 E. Lindsey Enders - u1250233,
 Tony Robinson - u0531330,
 Nathan Sartnurak - u1089358


