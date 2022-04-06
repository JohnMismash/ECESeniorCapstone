/* USER CODE BEGIN Header */
/** ECE 3992/4710 FINAL PROJECT CODE

PIN ASSIGNMENT:
PB10/PB11 - USART Interface
PB0 - Limit Switch Signal Pin
PC6-9 - LED Pins

V1 Implementation of Inter-board communication via USART.  Boards will speak to each other
		Testing will involve pressing a button to signal another STM board to turn on an LED

V2 Implementing Limit Switch to reset LED signal after USART connection is made.

	

  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
	* @authors				: John (Jack) Mismash, u1179865 - University of Utah - ECE 5780
	*										Andrew Porter, u1071655 - University of Utah - ECE 5780
	*										Tony Robinson, u - University of Utah - ECE 5780
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
#define STARTMOTOR 'S';
#define STOPMOTOR  0x55;
#define ACK				 'A';

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
void TransmitToTopBoard(char message);
void ReceiveFromTopBoard(void);
void LED_Init(void);
void USART_Init(void);
void LimitSwitch_Init(void);

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
	
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
	
  /* USER CODE BEGIN 2 */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	LED_Init();
	USART_Init();
	LimitSwitch_Init();
															 
  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  while (1){
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
  }
}

void LED_Init(void){
	__HAL_RCC_GPIOC_CLK_ENABLE();

	// INIT LED PINS.
	GPIO_InitTypeDef initStrLED = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
																 GPIO_MODE_OUTPUT_PP,
																 GPIO_SPEED_FREQ_LOW,
																 GPIO_NOPULL};
	
	// Initialize pins PC6 & PC7 & PC8 & PC9.
	HAL_GPIO_Init(GPIOC, &initStrLED); 
}

void USART_Init(void){
	__HAL_RCC_USART3_CLK_ENABLE();
	
	// INITIALIZE THE TX LINE.
	GPIO_InitTypeDef initStr = {GPIO_PIN_10,
														  GPIO_MODE_AF_PP,
														  GPIO_SPEED_FREQ_LOW,
														  GPIO_NOPULL};
	
	// INITIALIZE THE RX LINE.
	GPIO_InitTypeDef initStr1 = {GPIO_PIN_11,
															 GPIO_MODE_AF_PP,
															 GPIO_SPEED_FREQ_LOW,
															 GPIO_PULLUP};
	
	// INIT PIN 10 & 11.														 
	HAL_GPIO_Init(GPIOB, &initStr);
	HAL_GPIO_Init(GPIOB, &initStr1);
	
	// SETS ALTERNATE FUCTION 4 FOR PINS PB10 AND PB11
	GPIOB -> AFR[1] |= (1 << 10);
	GPIOB -> AFR[1] |= (1 << 14);

	// GET SYSTEM CLOCK FREQ.															
	HAL_RCC_GetHCLKFreq();
	
	// SET THE BAUD RATE TO 115200 BITS/SECOND
	USART3 -> BRR |= ((1 << 0) | (1 << 2) | (1 << 6));
															 
	// SETS THE USART ENABLE BIT 0 TO 1 (USART ENABLED).													 
	USART3 -> CR1 |= 1;

	// ENABLE TX AND RX
	USART3->CR1 |= (1 << 3);
	USART3->CR1 |= (1 << 2);
	
	// ENABLE THE USART
	USART3->CR1 |= (1 << 0);

	// ENABLE THE Receive Register Not Empty Interrupt (RNE)	.
	USART3 -> CR1 |= (1 << 5);
	
	// ENABLE THE USART PRIORITY IN THE NVIC.
	NVIC_EnableIRQ(USART3_4_IRQn);
}

void LimitSwitch_Init(void){
	// INIT PIN PB0 TO INPUT MODE.
	GPIO_InitTypeDef initStr = {GPIO_PIN_0,
														  GPIO_MODE_INPUT,
														  GPIO_SPEED_FREQ_LOW,
														  GPIO_PULLDOWN};
	// INIT PB0.
	HAL_GPIO_Init(GPIOB, &initStr);	

	// ENABLES RISING TRIGGER EVENT FOR PB0.														
	EXTI -> RTSR |= (1 << 0);														
	
	// CREATE AS A INTERRUPT SIGNAL, NOT EVENT SIGNAL.														
	EXTI -> IMR |= (1 << 0);

	// USE THE RCC TO ENABLE THE SYSCFG PERIPHERAL CLK														
	__HAL_RCC_SYSCFG_CLK_ENABLE();

	// ROUTE PB0 TO THE EXTI INPUT LINE 0 (EXTI0).														
	SYSCFG -> EXTICR[0] = 1;

	// ENABLE THE EXTI INTERRUPT.														
	NVIC_EnableIRQ(EXTI0_1_IRQn);															
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
	while((USART3 -> ISR & (1 << 7)) == 0){
		// DO NOTHING WHILE FOR USART STATUS FLAG TO SIGNAL IF TDR IS EMPTY.
	}
	
	// WRITES THE CHAR INTO THE TDR (Transmit Data Register).
	USART3 -> TDR = x;
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

//UART interupt handler
void USART3_4_IRQHandler(void){
	
	// RECEIVE DATA WHEN INTERRUPT HANDLER IS TRIGGERED.
	read_data = USART3 -> RDR;
	
	// BEGIN ACKNOWLEDGEMENT.
	ReceiveFromTopBoard();

	// RESET FLAGS
	USART3 -> ISR &= ~(1<<5); // RXNE REGISTER
	USART3 -> ISR &= ~(1<<3); // ORE REGISTER
}


//Limit switch interupt handler
void EXTI0_1_IRQHandler(void){
	// TODO! :)
	//turn off LED
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
}

/* Transmits a message to the top board. */
void TransmitToTopBoard(char message){
	
	/* MESSAGE FROM BOTTOM BOARD TO TOP BOARD. */
	TransmitChar(message);
}

/* Receives a message from the top board. */
void ReceiveFromTopBoard(void){
	// Read_data is STARTMOTOR Signal
	if(read_data == START_MOTOR){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		TransmitToTopBoard('A');				
	}
	
	// Read_data is ACK Signal
	if(read_data == 'A'){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		read_data = 0;
	}
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
