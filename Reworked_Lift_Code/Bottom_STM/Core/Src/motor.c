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
IN 2: PC0

IN 1 = HIGH, IN 2 = LOW: Forward direction
IN 1 = LOW, IN 2 = HIGH: Reverse direction

Controls:
PA4 = Enable

PA1 = Current sensing.

*/


// Sets up the entire motor drive system
void motor_init(void) {
	//pwm_init();
	basic_control_init();
	ADC_init();
	
}

// Sets up PA4 as a direct HIGH or LOW enable for the H-bridge
// Sets up PA5 and PC0 to be the DIRECTION inputs to the H-Bridge
void basic_control_init(void){
	//GPIOA->MODER = 0x28000000; //Reset value
	GPIOA->MODER &= ~((3 << 8) | (3 << 10)); //Zero out PA4 and PA5 in GPIOA
	GPIOA->MODER |= (1 << 8) | (1 << 10); //Set PA4 and PA5 as output
	GPIOC->MODER &= ~(3 << 0); //Zero out PC0 of GPIOC
	GPIOC->MODER |= (1 << 0); //PC0 as output
	
	GPIOA->ODR &= ~((1 << 4) | (1 << 5)); //Set PA4 and PA5 to OFF by default.
	GPIOC->ODR &= ~(1 << 0); //Set PC0 to OFF by default.
	
}

// Sets up the PWM and direction signals to drive the H-Bridge
void pwm_init(void) {
	/** TO DO -- Clean up and test PWM enable using the new pins **/
	
	/*
	// Set up pin PA4 for H-bridge PWM output (TIMER 14 CH1)
	GPIOA->MODER |= (1 << 9);
	GPIOA->MODER &= ~(1 << 8);

	// Set PA4 to AF4,
	GPIOA->AFR[0] &= 0xFFF0FFFF; // clear PA4 bits,
	GPIOA->AFR[0] |= (1 << 18);
	*/
	
	/*
	//For testing, set PA4 to a simple output instead.
	GPIOA->MODER = 0x28000000; //Reset value
	GPIOA->MODER |= (1 << 8) | (1 << 10); //Set PA4 as output
	//GPIOA->OTYPER = 0; //Reset state by default
	//GPIOA->OSPEEDR = 0x0C000000; //Reset state by default
	GPIOA->ODR = 0; //Set everything to OFF by default.
	//GPIOA->PUPDR = 0x24000000; //Reset value
	

	// Set up a PA5, PA6 as GPIO output pins for motor direction control
	//GPIOA->MODER &= 0xFFFFC3FF; // clear PA5, PA6 bits,
	//GPIOA->MODER |= (1 << 10);
	
	GPIOC->MODER = 0;
	GPIOC->OTYPER = 0;
	GPIOC->OSPEEDR = 0;
	GPIOC->PUPDR = 0;
	GPIOC->ODR = 0;
	
	GPIOC->MODER |= (1 << 0);
	*/
	
	//Initialize one direction pin to high, the other low
	//Default is forward direction.
	//GPIOA-> ODR = 0;
	//GPIOA->ODR |= (1 << 6);
	//GPIOA->ODR &= ~(1 << 5);

	/*
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
	*/
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
	
	//pwm_setDutyCycle(0);
	
	GPIOA->ODR &= ~(1 << 4); //Set PA4 to low.
	GPIOC->ODR &= ~(1 << 0); //Set PC0 to low.

	//Set PA5 to high for forward.
	GPIOA->ODR |= (1 << 5);
}

void motor_reverse(void){
	//pwm_setDutyCycle(0);
	GPIOA->ODR &= ~((1 << 4) | (1 << 5)); //Set PA4 and PA5 to low.
	//GPIOC->ODR &= ~(1 << 0); //Set PC0 to low.

	//Set PC0 to high for reverse.
	GPIOC->ODR |= (1 << 0);
}

void motor_run(uint8_t duty){
	//pwm_setDutyCycle(duty);
	GPIOA->ODR |= (1 << 4); //Enable the h-bridge
	read_current(); //Current should be checked periodically to ensure stall current not detected
}

void motor_hold(void){
	//pwm_setDutyCycle(0);
	GPIOA->ODR &= ~((1 << 4) | (1 << 5)); //Set PA4 and PA5 to low.
	GPIOC->ODR &= ~(1 << 0); //Set PC0 to low.
}

//Meaningful values TBD for our case.
void read_current(void){
		// Read the ADC value for current monitoring, actual conversion into meaningful units 
    // will be performed by STMStudio
    if(ADC1->ISR & ADC_ISR_EOC) {   // If the ADC has new data for us
        adc_value = ADC1->DR;       // Read the motor current for debug viewing
    }
}
