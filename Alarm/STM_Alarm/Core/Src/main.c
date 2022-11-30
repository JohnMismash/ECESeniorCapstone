/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
char data_1; 
char data_2;

volatile int num_of_char_send = 0;
volatile int verify_index = 0;


volatile int alarm_en = 0;

int8_t alarm_start = 0;

char input_char; 

char input_password[4]; 
char verify_password[4];


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void transmit_char(char c);
void transmit_string(char *c);
void USART3_4_IRQHandler(void);
void static GPIO_Init(void);
void BCD_7_Segment(int num);
void WHO_AM_I(void);


uint8_t byte_read(uint8_t addr);
void byte_write(uint8_t addr, uint8_t data);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t data1;
int8_t data2;
uint8_t status1;
int16_t xdata;

int16_t counter = 0;


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	__HAL_RCC_GPIOC_CLK_ENABLE(); // Enable the GPIOC clock in the RCC
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_USART3_CLK_ENABLE();
	HAL_RCC_GetHCLKFreq(); 
	__HAL_RCC_I2C2_CLK_ENABLE();
	
	/* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
	GPIO_Init();
	
	//Enable the timer 3 peripheral (TIM3) in the RCC.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	// Set up a configuration struct to pass to the initialization function
	GPIO_InitTypeDef initStr1 = {GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_6 | GPIO_PIN_7, 
															 GPIO_MODE_OUTPUT_PP,
															 GPIO_SPEED_FREQ_LOW,
															 GPIO_NOPULL};
	
	HAL_GPIO_Init(GPIOC, &initStr1); // Initialize pins 
	
	
	// Set up a configuration struct to pass to the initialization function
	GPIO_InitTypeDef initStr2 = {GPIO_PIN_10, 
															 GPIO_MODE_AF_PP,
															 GPIO_SPEED_FREQ_LOW,
															 GPIO_NOPULL};
	
	HAL_GPIO_Init(GPIOC, &initStr2); // Initialize pins 															
															
	// Set up a configuration struct to pass to the initialization function
	GPIO_InitTypeDef initStr3 = {GPIO_PIN_11, 
															 GPIO_MODE_AF_PP,
															 GPIO_SPEED_FREQ_LOW,
															 GPIO_PULLUP};
	
	HAL_GPIO_Init(GPIOC, &initStr3); // Initialize pins 	
	
	 
	// Set GPIOC to Alternate function 														 
	GPIOC->AFR[1] |= (1 << 8) | (1 << 12); 														 
															 
  USART3->BRR = 69;
								
  // USART ENABLE															 
	USART3->CR1 |= 1; 
	
	// USART Enable TX and RX
	USART3->CR1 |= (1 << 2); 
	USART3->CR1 |= (1 << 3); 
															 
  // 1. In the USART initialization, enable the receive register not empty interrupt
	USART3->CR1 |= (1 << 5);
	
	/////////////////////////////////// TIMER 3 Init: ///////////////////////////////////
	// Configuring Timer to 800 Hz 
	TIM3->ARR  = 250;
	TIM3->PSC  = 39;
	TIM3->DIER|= (1 << 0);
	TIM3->CR1 |= (1 << 0);
	TIM3->CR2 |= (1 << 4);

	// Use the Capture/Compare Mode Register 1 (CCMR1) register to configure the output channel to PWM mode.
	TIM3->CCMR1 &= ~(1 << 0);
	TIM3->CCMR1 &= ~(1 << 1);
	TIM3->CCMR1 &= ~(1 << 8);
	TIM3->CCMR1 &= ~(1 << 9);

	TIM3->CCMR1 |= (0x0070);

	TIM3->CCMR1 &= ~(1 << 12); 
	TIM3->CCMR1 |= (0x6000);

	// Pre-load 
	TIM3->CCMR1 |= (1 << 3) | (1 << 11);

	// 3.4: Set the output enable bits for channels 1 & 2 in the CCER register.
	TIM3->CCER |= (1 << 0) | (1 << 4);

	// 3.5: Set the capture/compare registers (CCRx) for both channels to 20% of your ARR value.
	TIM3->CCR1 = 150;
	TIM3->CCR2 = 150;

	GPIOC->MODER |= (1<<16) | (1<<18) | (1<<14) | (1<<12); 

	// Alternate Function Mode:
	GPIOC-> MODER |= (1 << 15) | (1 << 13);
	GPIOC-> MODER &= ~(1 << 14);
	GPIOC-> MODER &= ~(1 << 12);

	GPIOC->ODR |= (1<<9); 

	// Alternate function numbers
	GPIOC->AFR[0] &= 0x00FFFFFF;  

	//////////////////////////////////////////////////////////////////////
	
	
	/////////////////////////////////// Accelerometer Init: ///////////////////////////////////
	
	// Setting the GPIO Modes:  
	GPIOB->OTYPER = (1 << 11) | (1 << 13);	// Open Drain on PB11 and PB13
	GPIOB->MODER = (1 << 23) | (1 << 27) | (1 << 28);	// Mode 
	GPIOB->AFR[1] = (1 << 12) | (1 << 22) | (1 << 20); 
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); // Start PB14 high
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET); // Start PC0 high
	
	// Initializing the I2C Peripheral
	I2C2->TIMINGR = 0x10420F13;
	I2C2->CR1 = 1; // PE bit

	//WHO_AM_I();
	//////////////////////////////////////////////////////////////////////

															 
	// 2. Enable and set the USART interrupt priority in the NVIC.
	__NVIC_EnableIRQ(USART3_4_IRQn);
	__NVIC_SetPriority(USART3_4_IRQn, 1);	
	
	transmit_string("Enter 4 Digit password to start alarm:\r\n");	
															 
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		if(alarm_en == 1){
			
			//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET); 
			TIM3->ARR  = 250;
			TIM3->PSC  = 39;
			
			counter++;
			BCD_7_Segment(counter);
			
			//transmit_string("Alarm\r\n");
			
			volatile int i;
			for(i=0; i < 500000; i++){}
			
			
			//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET); 
			TIM3->ARR  = 0;
			for(i=0; i < 500000; i++){}			
				
		}
		
		else{
			TIM3->ARR  = 0;
			counter = 0;
			BCD_7_Segment(-1);
		}
		
		
	}
}


void USART3_4_IRQHandler(void){
	
	
	char data = USART3->RDR; 
	
	transmit_char(data);
	
	if(num_of_char_send == 4 && alarm_start == 0){
		transmit_string("\r\n");
	}
	
	if(verify_index == 4 && alarm_en == 1){
		transmit_string("\r\n");
	}
	
	input_char = data;
	
	if(num_of_char_send < 4 && alarm_start == 0){
		input_password[num_of_char_send] = data;
		num_of_char_send++;
		return;
	}
	
	if(alarm_en == 1 && verify_index < 4){
		verify_password[verify_index] = data; 
		verify_index++;
		return;
	}
	
	if(alarm_en == 1 && verify_index == 4 && data != 0x0D){
		verify_index = 0;
		return;
	}
	
	if(data == 0x0D && alarm_start == 0){
		
		alarm_start = 1;

		transmit_string("Alarm Armed\r\n");		
	
		// Alarm Being Enable. produce sound for 5 seconds
		for(int k = 5; k >= 0; --k){
			
			//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET); 
			TIM3->ARR  = 250;
			TIM3->PSC  = 39;
			
			BCD_7_Segment(k);
			
			volatile int i;
			for(i=0; i < 500000; i++){}
			
			
			//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET); 
			TIM3->ARR  = 0;
			for(i=0; i < 500000; i++){}	
			
			
		}
		
		BCD_7_Segment(-1);
		
		// Start Acceleormeter 
		while(1){
		
			status1 = byte_read(0x1E);
			
			//if(status1 == 0x06){
			data1 = byte_read(0x22);
			data2 = byte_read(0x23);
			
			//HAL_Delay(100);
			volatile int i;
			for(i=0; i < 100000; i++){}			
			
			xdata = (data2 << 8) | data1;
			
			if((-32766 <= xdata) && (xdata <= 400)){
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); 
			}
			
			else{
				
				transmit_string("Motion Detected...Please Enter 4 digit code to turn off:\r\n");	
				alarm_en = 1;	
				return; 
			}
		
		}
		
		
	}
	
	// Check to see if password is correct
	if(data == 0x0D && alarm_en == 1 && verify_index == 4){
		
		for(int i = 0; i < 4; i++){
			
			if(input_password[i] != verify_password[i]){
				
				verify_index = 0; 
				return; 
			}
			
			
		}
		
		alarm_en = 0; 
		verify_index = 0;
		alarm_start = 0;
		num_of_char_send = 0;
		transmit_string("Motion Detect Alarm OFF:\r\n");
		transmit_string("Enter 4 Digit password to start alarm:\r\n");
		
	}
				
}

void transmit_char(char c){

	// Transmit Data register Empty: 0 - data not transferred to shift register, 1 data is transferred 
	while(!(USART3->ISR & (1<<7))){ 
	}
	// Trasmit Data 
	USART3->TDR = c;
}

void transmit_string(char *c){
	int i = 0;  
	
	while (c[i] != '\0') {       /* Stop looping when we reach the null-character. */
    transmit_char(c[i]);
		i++;
	}	
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void static GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA4 PA5
                           PA8 PA9 PA10 PA11
                           PA12 PA13 PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void BCD_7_Segment(int num){
	
	
	if(num == -1){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET); 
		return;
	}
	
	num = num % 10;
	
	if(num == 0){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); 
	}
	
	else if(num == 1){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET); 
	}
	
	else if(num == 2){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); 
	}
	
	else if(num == 3){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); 
	}
	
	else if(num == 4){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET); 
	}
	
	else if(num == 5){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); 
	}
	
	else if(num == 6){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); 
	}
	
	else if(num == 7){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); 
	}
	
	else if(num == 8){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); 
	}
	
	else if(num == 9){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); 
	}
	
	
}

uint8_t byte_read(uint8_t addr)
{
		uint8_t val;
		// Reload the CR2 register with the same parameters as before, but set the RD_WRN bit to
		// indicate a read operation.	
	
		// Enable the X and Y sensing axes in the CTRL_REG1 register
		I2C2->CR2 &= ~((0x7F << 16) | (0x3FF << 0));
		I2C2->CR2 |= (1 << 16) | (0x6A << 1);		// Setting NBytes and Slave Address
		I2C2->CR2 &= ~(0x400);  // Clear RD_WRN to write transfer
		I2C2->CR2 |= (1 << 13);	// Start Bit 
		
		// Wait until either of the TXIS (Transmit Register Empty/Ready) or NACKF (Slave NotAcknowledge) flags are set.
		while(1){
			
			// TXIS
			if((I2C2->ISR & (1 << 1))){
				I2C2->TXDR = addr;
				break;
			}
			
			// NACKF
			if((I2C2->ISR & (1 << 4))){
				//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			}
			
		}
		
		//Wait until the TC (Transfer Complete) flag is set
		while(1){
			if((I2C2->ISR & (1 << 6))){
				break; 
			}
		}
		
		
		// Reload the CR2 register with the same parameters as before, but set the RD_WRN bit to
		// indicate a read operation.	
	
		I2C2->CR2 &= ~((0x7F << 16) | (0x3FF << 0));
		I2C2->CR2 |= (1 << 16) | (0x6A << 1) | (1 << 10);		
		I2C2->CR2 |= (1 << 13); // Start Bit
		
		// Wait until either of the RXNE (Receive Register Not Empty) or NACKF (Slave NotAcknowledge) flags are set.
		while (1)
		{
			if((I2C2->ISR & (1 << 2)) /*| (I2C2->ISR & (1 << 4))*/ ){
				break; 
			}
		}
	
	
		//Wait until the TC (Transfer Complete) flag is set
		while(1){
			if((I2C2->ISR & (1 << 6))){
				break; 
			}
		}
	
		val = I2C2->RXDR; 
		
		
		// Set the STOP bit in the CR2 register to release the I2C bus
		I2C2-> CR2 |= (1 << 14);
		
		return val;
}

void byte_write(uint8_t addr, uint8_t data)
{
	
	I2C2->CR2 &= ~((0x7F << 16) | (0x3FF << 0));
	I2C2->CR2 |= (2 << 16) | (0x6A << 1);		// Setting NBytes and Slave Address
	I2C2->CR2 &= ~(0x400);  // Clear RD_WRN to write transfer
	I2C2->CR2 |= (1 << 13);	// Start Bit 

	// Wait until either of the TXIS (Transmit Register Empty/Ready) or NACKF (Slave NotAcknowledge) flags are set.
	while(1){
		if((I2C2->ISR & (1 << 1)) | (I2C2->ISR & (1 << 4))){
			break; 
		}
	}
	
	//Write address 
	I2C2->TXDR = addr; 
	
	// Wait until either of the TXIS (Transmit Register Empty/Ready) or NACKF (Slave NotAcknowledge) flags are set.
	while(1){
		if((I2C2->ISR & (1 << 1)) | (I2C2->ISR & (1 << 4))){
				break; 
		}
	}
	
	// Write Data
	I2C2->TXDR = data;
	
	//Wait until the TC (Transfer Complete) flag is set
	while(1){
		if((I2C2->ISR & (1 << 6))){
				break; 
		}
	}

		
	// Set the STOP bit to release the I2C bus
	I2C2-> CR2 |= (1 << 14);
	
}


void WHO_AM_I(void){
	
	// Reading the Register 
	// 1. Set the L3GD20 slave address = 0x69
	I2C2->CR2 &= ~((0x7F << 16) | (0x3FF << 0));
	I2C2->CR2 |= (1 << 16) | (0x6A << 1);		
	I2C2->CR2 &= ~(0x400);
	I2C2->CR2 |= (1 << 13);	// Start Bit 														 
	

	// Wait until either of the TXIS (Transmit Register Empty/Ready) or NACKF (Slave NotAcknowledge) flags are set.
	while(1){
		if((I2C2->ISR & (1 << 1)) | (I2C2->ISR & (1 << 4))){
			break; 
		}
	}
	
	//Txis flag
	if(I2C2->ISR & (1 << 1)){
		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET); 
	}
	
	//Write the address of the ?WHO_AM_I? register into the I2C transmit register. (TXDR)
	I2C2->TXDR = 0x0F; 
	
	//Wait until the TC (Transfer Complete) flag is set
	while(1){
		if((I2C2->ISR & (1 << 6))){
			break; 
		}
	}
	
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET); // Start PB14 high
	
	// Reload the CR2 register with the same parameters as before, but set the RD_WRN bit to
  // indicate a read operation.
	//I2C2->CR2 |= 0;		
	I2C2->CR2 &= ~((0x7F << 16) | (0x3FF << 0));
	I2C2->CR2 |= (1 << 16) | (0x6A << 1) | (1 << 10);		
	I2C2->CR2 |= (1 << 13); // Start Bit
	
	// Wait until either of the RXNE (Receive Register Not Empty) or NACKF (Slave NotAcknowledge) flags are set.
	while (1)
  {
		if((I2C2->ISR & (1 << 2)) | (I2C2->ISR & (1 << 4))){
			break; 
		}
  }
	
	if(I2C2->ISR & (1 << 2)){
		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); 
	}
	
	//Wait until the TC (Transfer Complete) flag is set
	while(1){
		if((I2C2->ISR & (1 << 6))){
			break; 
		}
	}
	
	// Check the contents of the RXDR register to see if it matches 0xD4. (expected value of the ?WHO_AM_I? register)
	if(I2C2->RXDR == 0x6B){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	}
	
	// Set the STOP bit in the CR2 register to release the I2C bus
	I2C2-> CR2 |= (1 << 14);
	
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */



