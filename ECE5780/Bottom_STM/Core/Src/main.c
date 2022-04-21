/* USER CODE BEGIN Header */
/** ECE 3992/4710 FINAL PROJECT CODE - BOTTOM MICROCONTROLLER BOARD

PIN ASSIGNMENT:
PB10/PB11 - USART Interface
PA0 - Limit Switch Signal Pin
PC6-9 - LED Pins

---------------------------------------------------------------------------
HOW	TO WIRE THE LIMIT SWITCH:
1. Black wire goes to PA0 (It is NOT grounded!)
2. Yellow wire goes to 3V pin on board
---------------------------------------------------------------------------

---------------------------------------------------------------------------
HOW	TO WIRE THE UART CONNECTION:
1. Ground on top board goes to ground on bottom board
2. PB10 on Top Board goes to PB11 on Bottom Board
3. PB11 on Top Board goes to PB10 on Bottom Board
---------------------------------------------------------------------------

---------------------------------------------------------------------------
HOW TO WIRE THE MOTOR CONNECTION:
1. ENABLE on motor controller connects to PA4
2. INPUT 1 on the motor controller connects to PA5
3. INPUT 2 on the motor controller connects to PC0
4. I_SENSE on the motor controller connects to PA1 (Not yet tested).
---------------------------------------------------------------------------

V1 Implementation of Inter-board communication via USART.  Boards will speak to each other
		Testing will involve pressing a button to signal another STM board to turn on an LED

V2 Implementing Limit Switch to reset LED signal after USART connection is made.

V3 Integrating basic motor code and control with USART and limit switch triggers.

******************************************************************************
* @file           : main.c
* @brief          : Main program body for the BOTTOM microcontroller
* @authors				: John (Jack) Mismash, u1179865 - University of Utah - ECE 5780
*						  			Andrew Porter, u1071655 - University of Utah - ECE 5780
*						  			Tony Robinson, u0531330 - University of Utah - ECE 5780
*						  			Lindsey Enders, u1250233 - University of Utah - ECE 5780
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

#include "motor.h"

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

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char START_MOTOR = 'S';
char STOP_MOTOR = 'X';

char motorReverse = 0; //Flag that tells the system it can initiate lifting sequence.
char cycleTriggered = 0; //Prevents accidental limit switch trigger.
	
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
void TransmitChar (char );
void TransmitString (char* );
void TransmitToTopBoard(char);
void ReceiveFromTopBoard(uint8_t);
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
  __HAL_RCC_GPIOA_CLK_ENABLE();	
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
  LED_Init();
  USART_Init();
  LimitSwitch_Init();
	motor_init();
															 
  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  while (1){
	  HAL_Delay(200);
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
		
		if(motorReverse == 1 && cycleTriggered == 0){
			HAL_Delay(1000); //TO DO: Determine the delay for the lid at the bottom position.
			motor_reverse();
			motor_run(10);
			motorReverse = 0;
			cycleTriggered = 1; //Prevent triggering more raising if limit switch is bumped by something other than the lid.
		}
  }
}

// INITIALIZE LED PINS
void LED_Init(void){
	
	// ENABLE THE GPIOC
	__HAL_RCC_GPIOC_CLK_ENABLE();

	// INIT LED PINS.
	GPIO_InitTypeDef initStrLED = {GPIO_PIN_6| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
									GPIO_MODE_OUTPUT_PP,
									GPIO_SPEED_FREQ_LOW,
									GPIO_NOPULL};
	
	// INIT PINS PC6, PC7, PC8 AND PC9.
	HAL_GPIO_Init(GPIOC, &initStrLED); 
}

// INITIALIZE USART INTERFACE
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


// INITIALIZE LIMIT SWITCH INTERFACE
void LimitSwitch_Init(void){

	// USE THE RCC TO ENABLE THE SYSCFG PERIPHERAL CLK
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  // INIT PIN PA0 TO INPUT MODE.
  GPIO_InitTypeDef initStr = {GPIO_PIN_0,
															GPIO_MODE_INPUT,
															GPIO_SPEED_FREQ_LOW,
															GPIO_PULLUP};
	
  HAL_GPIO_Init(GPIOA, &initStr); // INIT PA0.

  EXTI -> IMR |= (1 << 0);         // CREATE AS A INTERRUPT SIGNAL, NOT EVENT SIGNAL.	
  EXTI -> RTSR |= (1 << 0);	     	 // ENABLES RISING TRIGGER EVENT FOR PA0.
  SYSCFG -> EXTICR[0] |= 0;        // ROUTE PA0 TO THE EXTI INPUT LINE 0 (EXTI0).
  NVIC_EnableIRQ(EXTI0_1_IRQn);    // ENABLE THE EXTI INTERRUPT.	
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


// USART INTERRUPT HANDLER
void USART3_4_IRQHandler(void){
	
	// RECEIVE DATA WHEN INTERRUPT HANDLER IS TRIGGERED.
	uint8_t read_data = USART3 -> RDR;
	
	// BEGIN ACKNOWLEDGEMENT.
	ReceiveFromTopBoard(read_data);

	// RESET FLAGS
	USART3 -> ISR &= ~(1<<5); // RXNE REGISTER
	USART3 -> ISR &= ~(1<<3); // ORE REGISTER
}



//(BOTTOM) LIMIT SWITCH INTERRUPT HANDLER
void EXTI0_1_IRQHandler(void){

	// TURN OFF LED
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
	
	// TODO: TURN OFF MOTOR
	motor_hold();
	motorReverse = 1;

	// TURN OFF THE INTERRUPT SIGNAL
	EXTI->PR |= (1<<0);
}


// TRANSMITS A MESSAGE TO THE TOP BOARD
void TransmitToTopBoard(char message){
	
	/* MESSAGE FROM BOTTOM BOARD TO TOP BOARD. */
	TransmitChar(message);
}



// RECEIVES A MESSAGE FROM THE TOP BOARD
void ReceiveFromTopBoard(uint8_t read_data){
	
	// read_data IS STARTMOTOR SIGNAL
	if(read_data == START_MOTOR){
		
		//START THE MOTOR
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		motor_forward();
		motor_run(10);
	}
	
	if (read_data == STOP_MOTOR){
		motor_hold();
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
