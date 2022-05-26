/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "st7735.h"
#include "stdio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
  uint8_t str[8];
  uint8_t dataReceived=0; // признак данное получено
   uint8_t dataTransmitted=1; // признак данное передано
 uint16_t  counter=0;
 volatile uint8_t OffOnDiod=0;
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
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */
void USARTADD()
{//Включаем тактирование UART1. Он подключен к шине APB1
	RCC->APB1ENR |= 0b1<<17; // включаем тактирование UART1
	//UART1 использует выводы: PA2 (для сигнала TX) и PA3 (сигнал RX). Надо задать конфигурацию для них.
	RCC->AHB1ENR |= 0b1;// разрешаем тактирование порта GPIOA

	GPIOA->MODER &=	~(0b11110000);

	GPIOA->MODER |= 0b100000;//PA2
	GPIOA->OTYPER &= ~(0b100);
	//GPIOA->OTYPER |= 0b100;
	//GPIOA->OTYPER |= 0b100;
	GPIOA->PUPDR &= ~(0b110000);
	GPIOA->PUPDR |= 0b010000;
	GPIOA->AFR[0] &= ~(0b111100000000);
	GPIOA->AFR[0] |= 0b011100000000;
	GPIOA->OSPEEDR |= 0b110000;

	GPIOA->MODER |= 0b10000000;//PA3
	GPIOA->OTYPER &= ~(0b1000);
	GPIOA->PUPDR &= ~(0b11000000);
	GPIOA->PUPDR |= 0b01000000;//PU//  mb PD?
	GPIOA->AFR[0] &= ~(0b1111000000000000);
	GPIOA->AFR[0] |= 0b0111000000000000;
	GPIOA->OSPEEDR |= 0b11000000;
	//GPIOA->BSRR |= 0b1 << 19;
	//GPIOA->ODR//proverit bez

	//sam usart
	USART2->CR1 = USART_CR1_UE;
	USART2->CR1 |= 0b1000;
	USART2->CR1 |= 0b100;
	//USART2->BRR = 8750;//9600 bod
	USART2->CR2 = 0;
	USART2->CR3 = 0;
	//USART2->

}
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void configGPIO()
{
	GPIOA->LCKR&= ~(0b1100);//тключение блокировки настройки регистров гпиоа
	//настройка регистров гпио а
		GPIOA->MODER &=	~(0b11110000);
		GPIOA->PUPDR &= ~(0b11110000);
		GPIOA->OTYPER &= ~(0b1100);
		GPIOA->OSPEEDR &= ~(0b11110000);
	////////////////////////////////////////////////////
///////////////////////////////////////////////////////

	RCC->AHB1ENR |= (1000);
	GPIOA->MODER &=	~(0b11);
	GPIOA->MODER |= 0b00;
	GPIOA->PUPDR &= ~(0b11);
	GPIOA->PUPDR |= 0b01;




	GPIOD->MODER &= ~(0b11 << 24);
	GPIOD->MODER |= 0b01<<24;
	//GPIOD->MODER |= GPIO_MODER_MODER12_0;
	GPIOD->OTYPER &= ~(0b1<<12);//GPIO_OTYPER_OT_12;
	GPIOD->OSPEEDR |= 0b1<<12;//GPIO_OSPEEDER_OSPEEDR12;

	GPIOD->MODER |= GPIO_MODER_MODER13_0;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_13;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13;

	GPIOD->MODER |= GPIO_MODER_MODER14_0;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_14;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR14;

	GPIOD->MODER |= GPIO_MODER_MODER15_0;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_15;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15;
}
/* USER CODE END 0 */


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

  if(huart == &huart2) {

    dataReceived=1;

    if( dataTransmitted != 0 ) {
      HAL_UART_Transmit_IT(&huart2, str, 8);
      dataReceived=0;
      dataTransmitted=0;
    }

    HAL_UART_Receive_IT (&huart2, str, 8);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {

  if(huart == &huart2) {

    dataTransmitted=1;

    if( dataReceived != 0 ) {
      HAL_UART_Transmit_IT(&huart2, str, 1);
      dataReceived=0;
      dataTransmitted=0;


    }
  }
}

void TimerSetting()
{
	RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;	//разрешить тактирование таймера 1
	TIM2->SMCR &= ~ TIM_SMCR_SMS;			//внутреннее тактирование таймера
	TIM2 -> CR1 = TIM_CR1_CEN;				//Устанавливаем режим работы таймера.
	TIM2->PSC = 839;
	TIM2->ARR = 24999;
	TIM2->DIER |= TIM_DIER_UIE;				//Разрешаем прерыванияв регистре разрешения таймера
	NVIC_EnableIRQ (TIM2_IRQn);			//Разрешаем прерыванияв регистре контроллера прерываний
}

char hello[]= {'P','a','r','a','m',' ','t','='};
char buffTemp[] = {'0','0','0','(','`','C',')'};
uint16_t ExperimentTime = 12345;//время эксперимента в секундах
uint16_t timerValue=0;


void ViewParam(uint16_t experimentTime)
{
	char mau[10];
	 sprintf(mau, "%d",ExperimentTime);
	 ST7735_DrawString(10,10,hello,sizeof hello,Font_11x18,ST7735_BLACK,ST7735_BLUE);
	 ST7735_DrawString(10,30,buffTemp,sizeof buffTemp,Font_11x18,ST7735_BLACK,ST7735_BLUE);
	 ST7735_DrawString(10,50,mau,sizeof mau,Font_11x18,ST7735_BLACK,ST7735_BLUE);
}

void SendImpuls()
{

}
int main(void)
{
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
 // MX_USART2_UART_Init();
 // TimerSetting();

//  ST7735_Init();
////  GPIOA->LCKR&=		  ~(0b1100);
// // GPIOA->MODER &=	  ~(0b11110000);
// // 	GPIOA->PUPDR &=	  ~(0b11110000);
// // 	GPIOA->OTYPER &=  ~(0b1100);
// // 	GPIOA->OSPEEDR &= ~(0b11110000);
  //MX_USART2_UART_Init();

//  ST7735_FillScreen(ST7735_BLUE);

  //configGPIO();


uint8_t mau;
MX_USART2_UART_Init();
  USARTADD();
  uint8_t d;
  while (1)
  {

	  while ((USART2->SR & USART_SR_RXNE) == 0)
	  {
		  mau = 1;
	  }
	       d = USART2->DR;

	  // отослать данное назад
	  while ((USART2->SR & USART_SR_TXE) == 0) {
		  mau = 2;
	  }
	  USART2->DR = d;
//	  ViewParam(ExperimentTime);
	 // HAL_UART_Receive_IT (&huart2, str, 1);
	 // ST7735_DrawString(10,10,str,Font_16x26,ST7735_BLACK,ST7735_BLUE);
	  //HAL_Delay(100);
	  //sprintf(displey,"%u",counter);
	 // ST7735_DrawString(10,10,displey,Font_16x26,ST7735_BLACK,ST7735_BLUE);

    /* USER CODE END WHILE */
//	  if( HAL_UART_Receive_IT (&huart2, str, 1) != HAL_BUSY ) {
//	    while( HAL_UART_Transmit_IT(&huart2, str, 1) == HAL_BUSY )
//	    	{
//	    		ST7735_DrawString(10,10,&str,Font_16x26,ST7735_BLACK,ST7735_BLUE);
//	    	};
//	    	}
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 *///////////////////////////////////////////PA2 TX white
	///////////////////////////////////////////////////////////////////////////PA3 RX purple

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, RS_Pin|A0_Pin|CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pins : RS_Pin A0_Pin CS_Pin */
  GPIO_InitStruct.Pin = RS_Pin|A0_Pin|CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PD13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
