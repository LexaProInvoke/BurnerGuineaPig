#include "main.h"

 volatile uint8_t OffOnDiod=0;
 uint16_t PSCTimer2 = 8399;
 uint16_t ARR = 9999;

void TimerSetting()
{
	RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;	//разрешить тактирование таймера 1
	TIM2->SMCR &= ~ TIM_SMCR_SMS;			//внутреннее тактирование таймера
	TIM2 -> CR1 = TIM_CR1_CEN;				//”станавливаем режим работы таймера.
	TIM2->PSC = PSCTimer2;//8399
	TIM2->ARR = ARR;//49999
	TIM2->DIER |= TIM_DIER_UIE;				//–азрешаем прерывани€в регистре разрешени€ таймера
	NVIC_EnableIRQ (TIM2_IRQn);			//–азрешаем прерывани€в регистре контроллера прерываний
}
//сделать микросекундный и секундный таймер
void WorkSignalGeneration()
{
	//84MGz / 8399 = 100 mcs * 49999 = 5 секунд
	// /
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
