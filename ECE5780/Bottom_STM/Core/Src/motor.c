/* -------------------------------------------------------------------------------------------------------------
 *  Motor Control and Initialization Functions
 * -------------------------------------------------------------------------------------------------------------
 */
#include "motor.h"

volatile uint8_t duty_cycle = 0;    	// Output PWM duty cycle
volatile int8_t adc_value = 0;      	// ADC measured motor current
volatile int16_t temp;

/*

Direction definitions:

IN 1: PA5
IN 2: PA6

IN 1 = HIGH, IN 2 = LOW: Forward direction
IN 1 = LOW, IN 2 = HIGH: Reverse direction

*/


// Sets up the entire motor drive system
void motor_init(void) {
	pwm_init();
	ADC_init();
	
}

// Sets up the PWM and direction signals to drive the H-Bridge
void pwm_init(void) {
	
	// Set up pin PA4 for H-bridge PWM output (TIMER 14 CH1)
	GPIOA->MODER |= (1 << 9);
	GPIOA->MODER &= ~(1 << 8);

	// Set PA4 to AF4,
	GPIOA->AFR[0] &= 0xFFF0FFFF; // clear PA4 bits,
	GPIOA->AFR[0] |= (1 << 18);

	// Set up a PA5, PA6 as GPIO output pins for motor direction control
	GPIOA->MODER &= 0xFFFFC3FF; // clear PA5, PA6 bits,
	GPIOA->MODER |= (1 << 10) | (1 << 12);
	
	//Initialize one direction pin to high, the other low
	GPIOA->ODR |= (1 << 5);
	GPIOA->ODR &= ~(1 << 6);

	// Set up PWM timer
	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
	TIM14->CR1 = 0;                         // Clear control registers
	TIM14->CCMR1 = 0;                       // (prevents having to manually clear bits)
	TIM14->CCER = 0;

	// Set output-compare CH1 to PWM1 mode and enable CCR1 preload buffer
	TIM14->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE);
	TIM14->CCER |= TIM_CCER_CC1E;           // Enable capture-compare channel 1
	TIM14->PSC = 1;                         // Run timer on 24Mhz
	TIM14->ARR = 1200;                      // PWM at 20kHz
	TIM14->CCR1 = 0;                        // Start PWM at 0% duty cycle
	
	TIM14->CR1 |= TIM_CR1_CEN;              // Enable timer
}

// Set the duty cycle of the PWM, accepts (0-100)
void pwm_setDutyCycle(uint8_t duty) {
		
	if(duty == 0){
		TIM14->CCR1 = 0; //Turn off duty cycle
	}
	else if(duty <= 100) {
			TIM14->CCR1 = ((uint32_t)duty*TIM14->ARR)/100;  // Use linear transform to produce CCR1 value
			// (CCR1 == "pulse" parameter in PWM struct used by peripheral library)
	}
}

void ADC_init(void) {

	// Configure PA1 for ADC input (used for current monitoring)
	GPIOA->MODER |= (GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1);

	// Configure ADC to 8-bit continuous-run mode, (asynchronous clock mode)
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

	ADC1->CFGR1 = 0;                        // Default resolution is 12-bit (RES[1:0] = 00 --> 12-bit)
	ADC1->CFGR1 |= ADC_CFGR1_CONT;          // Set to continuous mode
	ADC1->CHSELR |= ADC_CHSELR_CHSEL1;      // Enable channel 1

	ADC1->CR = 0;
	ADC1->CR |= ADC_CR_ADCAL;               // Perform self calibration
	while(ADC1->CR & ADC_CR_ADCAL);         // Delay until calibration is complete

	ADC1->CR |= ADC_CR_ADEN;                // Enable ADC
	while(!(ADC1->ISR & ADC_ISR_ADRDY));    // Wait until ADC ready
	ADC1->CR |= ADC_CR_ADSTART;             // Signal conversion start
}


void motor_forward(void){
	//First, set EN (duty cycle) to 0 to prevent any potential locking of motor (where input pins are equal)
	//and prevent sudden direction changes (bad for motor internals).
	pwm_setDutyCycle(0);
	
	GPIOB->BSRR |= (1 << 5) | (1 << 22);
}

void motor_reverse(void){
	pwm_setDutyCycle(0);
	
	GPIOB->BSRR |= (1 << 6) | (1 << 21);
}

void motor_run(uint8_t duty){
	pwm_setDutyCycle(duty);
	read_current(); //Current should be checked periodically to ensure stall current not detected
}

void motor_hold(void){
	pwm_setDutyCycle(0);
}

//Meaningful values TBD for our case.
void read_current(void){
		// Read the ADC value for current monitoring, actual conversion into meaningful units 
    // will be performed by STMStudio
    if(ADC1->ISR & ADC_ISR_EOC) {   // If the ADC has new data for us
        adc_value = ADC1->DR;       // Read the motor current for debug viewing
    }
}
