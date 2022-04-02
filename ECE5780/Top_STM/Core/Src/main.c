/* USER CODE BEGIN Header */
/** This is actually our final project code


V1 Implementation of Inter-board communication via UART.  Boards will speak to each other
		Testing will involve pressing a button to signal another STM board to turn on an LED
V2
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
	* @author					: John (Jack) Mismash, u1179865 - University of Utah - ECE 5780
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
#define STARTMOTOR 0x44;
#define STOPMOTOR  0x55;
#define ACK				 0xFF;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
volatile uint8_t read_data;
char START_MOTOR = 'S';

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
void TransmitChar (char x);
void TransmitString (char* x);
void TransmitToBottomBoard(void);
void ReceiveFromBottomBoard(void);
void setupDistanceSensor();

int READY_TO_GO;
	
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_USART3_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
	

  //enable LEDs
  RCC->AHBENR |=  RCC_AHBENR_GPIOCEN;
  GPIOC->MODER |= (1 << 12);   //red
  GPIOC->MODER |= (1 << 14);   //blue
  GPIOC->MODER |= (1 << 16);   //green
  GPIOC->MODER |= (1 << 18);   //orange


  // set up the ADC for the sensor
  //The Sensor uses PC0 for input
  setupDistanceSensor();



  /* USER CODE BEGIN 2 */
	//Initialize LED PINS
	GPIO_InitTypeDef initStrLED = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
																 GPIO_MODE_OUTPUT_PP,
																 GPIO_SPEED_FREQ_LOW,
																 GPIO_NOPULL};
	
	// INIT PINS PC8 & PC9 & PC6 * PC7
	HAL_GPIO_Init(GPIOC, &initStrLED);
	
	// INITIALIZE THE TX LINE.
	GPIO_InitTypeDef initStr = {GPIO_PIN_10,
															GPIO_MODE_AF_PP,
															GPIO_SPEED_FREQ_LOW,
															GPIO_NOPULL};
	
	// THIS IS THE RX LINE.
	GPIO_InitTypeDef initStr1 = {GPIO_PIN_11,
															 GPIO_MODE_AF_PP,
															 GPIO_SPEED_FREQ_LOW,
															 GPIO_PULLUP};
	
	// INIT PIN 10 & 11.
	HAL_GPIO_Init(GPIOB, &initStr);
	HAL_GPIO_Init(GPIOB, &initStr1);
	
	//SETS ALTERNATE FUCTION 4 FOR PINS PB10 AND PB11
	GPIOB -> AFR[1] |= (1 << 10);
	GPIOB -> AFR[1] |= (1 << 14);

	HAL_RCC_GetHCLKFreq();
	
	// SET THE BAUD RATE TO 115200 BITS/SECOND
	USART3 -> BRR |= ((1 << 0) | (1 << 2) | (1 << 6));
															 
	// SET THE RECEIVER ENABLE BIT 2 TO 1 
	// (Receiver is enabled and begins searching for a start bit).														 
	USART3 -> CR1 |= (1 << 2);
															 
	// SET THE TRANMITTER ENABLE BIT 3 TO 1
	// (Transmitter is enabled).
	USART3 -> CR1 |= (1 << 3);
															 
	// ENABLE THE Receive Register Not Empty Interrupt (RNE)	.											
	USART3 -> CR1 |= (1 << 5);														 

	// ENABLE THE receive register not empty interrupt
	USART3 -> CR1 |= (1 << 5);
	
	// ENABLE THE USART PRIORITY IN THE NVIC.
	NVIC_EnableIRQ(USART3_4_IRQn);
															 
															 
		
  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  int readVal;
  int count = 0;
  READY_TO_GO = 1;
  
  while (1){

    //range on readVal is aprox between 500 and 3000
	readVal = ADC1->DR; 
    if(readVal > 2000){
      count++;
    }
    else{
      GPIOC->ODR &= ~(1<<6);  //red
      GPIOC->ODR &= ~(1<<7);  //blue
      GPIOC->ODR &= ~(1<<8);  //orange
      GPIOC->ODR &= ~(1<<9);  //green
      count=0;
    }

    // If we get to this, we have had success and the sensor has picked up the package. So next step
    if( (count>100) && READY_TO_GO){
      GPIOC->ODR |= (1<<6);  //red
      GPIOC->ODR |= (1<<7);  //blue
      GPIOC->ODR |= (1<<8);  //orange
      GPIOC->ODR |= (1<<9);  //green
       
	  READY_TO_GO = 0;
      TransmitToBottomBoard();  
    }

  }
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
  while (1){}
		
  /* USER CODE END Error_Handler_Debug */
}

/* Transmits a single character using USART communication. */
void TransmitChar (char x){
	while((USART3->ISR & (1 << 7)) == 0){
		// Do nothing while data is not transferred to the shift register
	}
	
	USART3->TDR = x;
}

/* Transmits a single string using USART communication. */
void TransmitString (char* x){
	char* p = x;
	
	while(*p != 0){
		TransmitChar(*p);
		p += 1;
	}
		return;
}
/* USER CODE END 4 */

void USART3_4_IRQHandler(void){
	
	// RECEIVE DATA WHEN INTERRUPT HANDLER IS TRIGGERED.
	read_data = USART3 -> RDR;

	// BEGIN ACKNOWLEDGEMENT.
	ReceiveFromBottomBoard();

	// RESET FLAGS
	USART3 -> ISR &= ~(1<<5);// RXNE REGISTER
	USART3 -> ISR &= ~(1<<3);// ORE REGISTER
	
}

/* Transmits a message to the bottom board. */
void TransmitToBottomBoard(void){
	
	/* MESSAGE FROM TOP BOARD TO BOTTOM BOARD */
	TransmitChar(START_MOTOR);
}

/* Receives a message from the bottom board. */
void ReceiveFromBottomBoard(){
	
	// Read_data is STARTMOTOR Signal
	if(read_data == START_MOTOR){
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
		TransmitChar(0xFF);				
	}
	
	// Read_data is ACK Signal
	if(read_data == 'A'){
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
		read_data = 0;
	}
}


//This does all the setup for the Analog to Digital converter that is used for the distance sensor
void setupDistanceSensor(){

  RCC->AHBENR |=  RCC_AHBENR_GPIOCEN;

  //USE PC0
  //This is for enabling PC0
  GPIOC->MODER |= (0x3<<0);   // 11 in bit positions 1 and 0
  GPIOC->PUPDR &= ~(0x3<<0); 

  //Enable the ADC1 in the RCC peripheral.
  RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

  //Configure the ADC to 8-bit resolution, continuous conversion mode, hardware triggers disabled (software trigger only).
  ADC1->CFGR1 &= ~(0x3<<3);  //8 bits
  ADC1->CFGR1 |= (0x1<<4);   //8 bits
  ADC1->CFGR1 |= (0x1<<13);   //continuous conversion mode
  ADC1->CFGR1 &= ~(0x3<<3) ; //hardware triggers disabled (software trigger only)

  //Select/enable the input pin’s channel for ADC conversion.
  ADC1->CHSELR |= (1<<10);  // ADC_IN10 correlates to PC0   Search ADC_IN10 in the lab manual
  

  //Perform a self-calibration, enable, and start the ADC.

  //Calibration software procedure
  //1. Ensure that ADEN=0 and DMAEN=0 
  //2. Set ADCAL=1
  //3. Wait until ADCAL=0
  //4. The calibration factor can be read from bits 6:0 of ADC_DR.
  
  //1
  if ( ((ADC1->CR & 0x1) >> 0) != 0){
    //ERROR
    GPIOC->ODR |= (1<<6);
  }

  if ( ((ADC1->CFGR1 & 0x1) >> 0) != 0){
    //ERROR
    GPIOC->ODR |= (1<<6);
  }
  
  //2
  ADC1->CR |= (0x1 << 31);

  //3
  while(((ADC1->CR & 0x80000000) >> 30) != 0){

  }
  


  //Follow this procedure to enable the ADC:
  // 1. Clear the ADRDY bit in ADC_ISR register by programming this bit to 1.
  // 2. Set ADEN=1 in the ADC_CR register.
  // 3. Wait until ADRDY=1 in the ADC_ISR register and continue to write ADEN=1 (ADRDY is set after the ADC startup time). 
  //    This can be handled by interrupt if the interrupt is enabled by setting the ADRDYIE bit in the ADC_IER register.
  
  //1
  ADC1->ISR |= (0x1 << 0);

  //2
  ADC1->CR |= (0x1<<0);  //enable

  //3
  if ( ((ADC1->ISR & 0x1) >> 0) != 1){

  }

  ADC1->CR |= (0x1<<2);  //start
  
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
