//#include "main.h"
#include "HeatingControl.h"
 volatile uint8_t offOnDiod = 0;
 volatile uint32_t countdownHeatingTime  = 0;
 volatile uint16_t ARRHighLevelSignal = 9999;
 volatile uint16_t ARRLowLevelSignal = 9999;
 volatile uint16_t PSCHighLevelSignal = 8399;
 volatile uint16_t PSCLowLevelSignal = 8399;
 volatile uint8_t ControllerError = 0;
// volatile uint32_t DelayTime = 0;
//void MicrosecondTimer()//10 microsecond
//{
//		RCC -> APB1ENR |= RCC_APB1ENR_TIM4EN;	//разрешить тактирование таймера 2
//		TIM4->SMCR &= ~ TIM_SMCR_SMS;			//внутреннее тактирование таймера
//		//TIM3 -> CR1 = TIM_CR1_CEN;				//Устанавливаем режим работы таймера and start.
//		TIM4->PSC = 839;//8399
//		TIM4->ARR = 1;//9999
//		TIM4->DIER |= TIM_DIER_UIE;				//Разрешаем прерыванияв регистре разрешения таймера
//		NVIC_EnableIRQ (TIM4_IRQn);			//Разрешаем прерыванияв регистре контроллера прерываний
//		StopOneSecondTimer();
//}
//void StartMicrosecondTimer()
//{
//	TIM4 -> CR1 |= TIM_CR1_CEN;
//}
//void StopMicrosecondTimer()
//{
//	TIM4 -> CR1 &= ~(TIM_CR1_CEN);
//}
void OneSecondTimerSetting()
{
	RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;	//разрешить тактирование таймера 2
	TIM3->SMCR &= ~ TIM_SMCR_SMS;			//внутреннее тактирование таймера
	//TIM3 -> CR1 = TIM_CR1_CEN;				//Устанавливаем режим работы таймера and start.
	TIM3->PSC = 8399;//8399
	TIM3->ARR = 9999;//9999
	TIM3->DIER |= TIM_DIER_UIE;				//Разрешаем прерыванияв регистре разрешения таймера
	NVIC_EnableIRQ (TIM3_IRQn);			//Разрешаем прерыванияв регистре контроллера прерываний
	StopOneSecondTimer();
}
void SettingHeatingTime(uint32_t NewTime)//секундный обратный отсчет
{
	countdownHeatingTime = NewTime;
}
void StartOneSecondTimer()
{
	TIM3 -> CR1 |= TIM_CR1_CEN;
}
void StopOneSecondTimer()
{
	TIM3 -> CR1 &= ~(TIM_CR1_CEN);
}
void FrequencyTimerSetting()
{
	RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;	//разрешить тактирование таймера 2
	TIM2->SMCR &= ~ TIM_SMCR_SMS;			//внутреннее тактирование таймера
	//TIM2 -> CR1 = TIM_CR1_CEN;				//Устанавливаем режим работы таймера and start.
	TIM2->PSC = 8399;//8399
	TIM2->ARR = 9999;//9999
	TIM2->DIER |= TIM_DIER_UIE;				//Разрешаем прерыванияв регистре разрешения таймера
	NVIC_EnableIRQ (TIM2_IRQn);			//Разрешаем прерыванияв регистре контроллера прерываний
	StopSignalForHeating();
}
//сделать микросекундный и секундный таймер
uint8_t SettingFrequencyOutputSignal(uint16_t PSCHighLevel, uint16_t PSCLowLevel, uint16_t ARRHighLevel, uint16_t ARRLowLevel)
{
	if((PSCHighLevel < 65000) && (PSCHighLevel > 0) && (PSCLowLevel <65000) && (PSCLowLevel > 0) &&(ARRHighLevel < 65000) && (ARRHighLevel > 0) && (ARRLowLevel <65000) && (ARRLowLevel > 0))
	{
		ARRHighLevelSignal 	= ARRHighLevel;
		ARRLowLevelSignal 	= ARRLowLevel;
		PSCHighLevelSignal	= PSCHighLevel;
		PSCLowLevelSignal 	= PSCLowLevel;
		return 1;
	}
	else
	{
		return 0;
	}
	//84MGz / 8399 = 100 mcs * 9999 = 1 секунда//функция настройки частоты сигнала выхода
}
void StartSignalForHeating()
{
	TIM2 -> CR1 |= TIM_CR1_CEN;
}
void StopSignalForHeating()
{
	TIM2 -> CR1 &= ~(TIM_CR1_CEN);
	GPIOD->ODR &= ~(0b1<<12);//потушить светодиод
	//+gpio v 0
}

//void UpTemperature()
//{
//	if(ARRHighLevelSignal > 64999)
//	{
//		if(ARRLowLevelSignal < 2)
//		{
//			if(PSCHighLevelSignal > 64999)
//			{
//				if(PSCLowLevelSignal < 2)
//				{
//					//error
//					StopSignalForHeating();
//					ControllerError = ErrorInFrequencySelection;
//				}
//				else
//				{
//					PSCLowLevelSignal--;
//					ARRHighLevelSignal = 9999;
//					ARRLowLevelSignal = 9999;
//				}
//			}
//			else
//			{
//				PSCHighLevelSignal++;
//				ARRHighLevelSignal = 9999;
//				ARRLowLevelSignal = 9999;
//			}
//
//		}
//		else
//		{
//		ARRLowLevelSignal --;
//		}
//	}
//	else
//	{
//		ARRHighLevelSignal++;
//	}
//
//
//
//
//}
//void DownTemperature()
//{
//	if(ARRLowLevelSignal > 64999)
//		{
//			if(ARRHighLevelSignal < 2)
//			{
//				if(PSCLowLevelSignal > 64999)
//				{
//					if(PSCHighLevelSignal < 2)
//					{
//						//error
//						StopSignalForHeating();
//						ControllerError = ErrorInFrequencySelection;
//					}
//					else
//					{
//						PSCHighLevelSignal--;
//						ARRHighLevelSignal = 9999;
//						ARRLowLevelSignal = 9999;
//					}
//				}
//				else
//				{
//					PSCLowLevelSignal++;
//					ARRHighLevelSignal = 9999;
//					ARRLowLevelSignal = 9999;
//				}
//
//			}
//			else
//			{
//			ARRHighLevelSignal --;
//			}
//		}
//		else
//		{
//			ARRLowLevelSignal++;
//		}
//}
//void AutoFrequencySetting(uint8_t TemperatureIsCorrect)
//{
//	if(TemperatureIsCorrect == TemperatureIsHigher)
//	{
//		DownTemperature();
//	}
//	else
//	{
//		UpTemperature();
//	}
//}


void configGPIO()
{
	GPIOA->LCKR&= ~(0b1100);
	GPIOA->MODER &=	~(0b11110000);
	GPIOA->PUPDR &= ~(0b11110000);
	GPIOA->OTYPER &= ~(0b1100);
	GPIOA->OSPEEDR &= ~(0b11110000);
	////////////////////////////////////////////////////

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
