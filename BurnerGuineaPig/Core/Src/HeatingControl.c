//#include "main.h"
#include "HeatingControl.h"
 volatile uint8_t offOnDiod = 0;
 volatile uint32_t countdownHeatingTime  = 0;
 volatile uint32_t TimeHighLevel = 5;
 volatile uint32_t TimeLowLevel = 5;
 volatile uint32_t TimeHighLevelBuff = 10;
 volatile uint32_t TimeLowLevelBuff = 10;
 volatile uint8_t ControllerError = 0;
 volatile uint16_t PSCFrequencyTimer = 839;
 volatile uint16_t ARRFrequencyTimer = 9999;
void OneSecondTimerSetting()
{
	RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;	//разрешить тактирование таймера 2
	TIM3->SMCR &= ~ TIM_SMCR_SMS;			//внутреннее тактирование таймера
	//TIM3 -> CR1 = TIM_CR1_CEN;				//”станавливаем режим работы таймера and start.
	TIM3->PSC = 8399;//8399
	TIM3->ARR = 9999;//9999
	TIM3->DIER |= TIM_DIER_UIE;				//–азрешаем прерывани€в регистре разрешени€ таймера
	NVIC_EnableIRQ (TIM3_IRQn);			//–азрешаем прерывани€в регистре контроллера прерываний
	StopOneSecondTimer();
	NVIC_SetPriority(TIM3_IRQn, 1);
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
	//TIM2 -> CR1 = TIM_CR1_CEN;				//”станавливаем режим работы таймера and start.
	TIM2->PSC = 839;//8399  84000000/840 = 100000
	TIM2->ARR = 9999;//9999
	TIM2->DIER |= TIM_DIER_UIE;				//–азрешаем прерывани€в регистре разрешени€ таймера
	NVIC_EnableIRQ (TIM2_IRQn);			//–азрешаем прерывани€в регистре контроллера прерываний
	NVIC_SetPriority(TIM2_IRQn, 2);
	TIM2 -> CR1 &= ~(TIM_CR1_CEN);
}
//сделать микросекундный и секундный таймер

void StartSignalForHeating()
{
	TIM2 -> CR1 |= TIM_CR1_CEN;
	IndicationStartHeating();
}
void StopSignalForHeating()
{
	TIM2 -> CR1 &= ~(TIM_CR1_CEN);
	GPIOD->ODR &= ~(0b1<<12);//потушить светодиод
	IndicationStopHeating();
	numberCommand = Stop_Heating;
	//+gpio v 0
}

void UpTemperature()
{
	if(TimeHighLevel>9999)
	{
		if(TimeLowLevel==0)
		{
			ControllerError = ErrorTemperatureOverflow;
		}
		else
		{
			TimeLowLevel--;
		}
	}
	else
	{
		TimeHighLevel++;
		if(TimeLowLevel==0)
		{
			ControllerError = ErrorTemperatureOverflow;
		}
		else
		{
			TimeLowLevel--;
		}
	}
	TimeHighLevelBuff = TimeHighLevel;
	TimeLowLevelBuff = TimeLowLevel;
}
void DownTemperature()//TimeHighLevel
{
		if(TimeLowLevel>9999)
		{
			if(TimeHighLevel==0)
			{
				ControllerError = ErrorTemperatureOverflow;
			}
			else
			{
				TimeHighLevel--;
			}
		}
		else
		{
			TimeLowLevel++;
			if(TimeHighLevel==0)
			{
				ControllerError = ErrorTemperatureOverflow;
			}
			else
			{
				TimeHighLevel--;
			}
		}
		TimeHighLevelBuff = TimeHighLevel;
		TimeLowLevelBuff = TimeLowLevel;
}
void AutoFrequencySetting(uint8_t TemperatureIsCorrect)
{
	if(TemperatureIsCorrect == TemperatureIsHigher)
	{
		DownTemperature();
	}
	else
	{
		UpTemperature();
	}
}


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
