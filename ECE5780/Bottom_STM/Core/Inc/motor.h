
#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include "stm32f0xx.h"

/* -------------------------------------------------------------------------------------------------------------
 *  Global Variable and Type Declarations
 *  -------------------------------------------------------------------------------------------------------------
 */
extern volatile uint8_t duty_cycle;    // Output PWM duty cycle
extern volatile int8_t adc_value;      // ADC measured motor current
extern volatile int16_t temp;					//Temp read from temp sensor.


/* -------------------------------------------------------------------------------------------------------------
 *  Motor Control and Initialization Functions
 * -------------------------------------------------------------------------------------------------------------
 */

// Sets up the entire motor drive system
void motor_init(void);

// Sets H-bridge pins for forward motor motion
void motor_forward(void);

// Sets H-bridge pins for backward motor motion
void motor_reverse(void);

// Turns on the motor to run with the PWM at the specified duty cycle
void motor_run(uint8_t duty);

//SOFT stop of motor (turns off enable)
void motor_hold(void);

//Reads the measured ADC value
void read_current(void);

// Set the duty cycle of the PWM, accepts (0-100)
void pwm_setDutyCycle(uint8_t duty);

/* -------------------------------------------------------------------------------------------------------------
 *  Temperature Sensor Reading
 * -------------------------------------------------------------------------------------------------------------
 */

// Reads current temperature (9-bit signed value, will be in upper bits of 16-bit register.)
void read_temp(void);

/* -------------------------------------------------------------------------------------------------------------
 *  Internal-Use Initialization Functions
 * -------------------------------------------------------------------------------------------------------------
 */

// Sets up the PWM and direction signals to drive the H-Bridge
void pwm_init(void);

//Sets up basic H-bridge controls (no PWM)
void basic_control_init(void);

// Sets up ADC to measure motor current
void ADC_init(void);

//Initializes the temperature sensor
void temp_sensor_init(void);

//Initializes the I2C line to communicate with the temp sensor
//at the specified address. Address will be 1001 A2 A1 A0.
//Expected value will be 1001111 based on our board config.
void I2C_init(char address);

#endif /* MOTOR_H_ */
