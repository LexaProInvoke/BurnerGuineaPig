#include "UsartProtocol.h"


states_Control_Device state_Control_Device = 0;
volatile uint16_t experimentNumber = 0;
volatile uint8_t flugOfEndCommand = 0;//флаг когда закончилась команда.
uint8_t commandCheckFlag = CommandNotIdentified;//or CommandIdentified
uint8_t stringWithCommand[LenghtTXString];
uint8_t buffStringWithCommand[LenghtTXString];
uint8_t counterTXGetByte = 0;

char IntstringForSendToPC[32] = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
//uint8_t IntstringForSendToPC[32]  ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
volatile char StringForSendToPC[32];
uint8_t indicateFirstInput=0;

volatile uint8_t TemperatureIsCorrect = TemperatureCorrect;

volatile int numberCommand = 0;
int TIM2ARR = 0;
int TIM2PSC = 0;
uint32_t TimeFromBuffer = 0;
volatile uint16_t TemperatureFromBuffer = 0;

void USART_Init()
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
	USART2->CR1 |= 0b1000;//разрешение отправки сообщений
	USART2->CR1 |= 0b100;//разрешение принятия сообщений
	USART2->CR1 |= 0b100000;//разрешение прерываний
	USART2->BRR = 4375;//9600 bod скорость передачи берется или из тебляци или расчитывается по формуле
	USART2->CR2 = 0;
	USART2->CR3 = 0;
	//USART2->
	NVIC_EnableIRQ (USART2_IRQn);//разрешение прерыwаний усарт
	NVIC_SetPriority(USART2_IRQn, 0);
}
void PreparCommandToSentComputer()
{
	char experimentNumberMusForSend[4] = {'0','0','0','0'};
	sprintf(experimentNumberMusForSend,"%04u",experimentNumber);
	char timeMusForSend[5] = {'0','0','0','0','0'};
	sprintf(timeMusForSend,"%05u",countdownHeatingTime);
	char tempMusForSend[3] = {'0','0','0'};
	uint16_t TemperatureForPC = SensorTemperature;
	sprintf(tempMusForSend,"%03u",TemperatureForPC);
	char PSCmusForSend[5] = {'0','0','0','0','0'};
	sprintf(PSCmusForSend,"%05u",PSCFrequencyTimer);
	char ARRmusForSend[5] = {'0','0','0','0','0'};
	sprintf(ARRmusForSend,"%05u",ARRFrequencyTimer);
	IntstringForSendToPC[0] = '8';
	IntstringForSendToPC[1]= numberCommand/10 + '0';//nomberCommand
	IntstringForSendToPC[2]= numberCommand%10 + '0';
	IntstringForSendToPC[3]=experimentNumberMusForSend[0];
	IntstringForSendToPC[4]=experimentNumberMusForSend[1];
	IntstringForSendToPC[5]=experimentNumberMusForSend[2];
	IntstringForSendToPC[6]=experimentNumberMusForSend[3];
	IntstringForSendToPC[7]= PSCmusForSend[0];
	IntstringForSendToPC[8]= PSCmusForSend[1];
	IntstringForSendToPC[9]= PSCmusForSend[2];
	IntstringForSendToPC[10]=PSCmusForSend[3];
	IntstringForSendToPC[11]=PSCmusForSend[4];
	IntstringForSendToPC[12]=ARRmusForSend[0];
	IntstringForSendToPC[13]=ARRmusForSend[1];
	IntstringForSendToPC[14]=ARRmusForSend[2];
    IntstringForSendToPC[15]=ARRmusForSend[3];
    IntstringForSendToPC[16]=ARRmusForSend[4];
	//PSCFrequencyTimer; PSC|7|8|9|10|11|
	//ARRFrequencyTimer; ARR12|13|14|15|16
    IntstringForSendToPC[17]=tempMusForSend[0];
    IntstringForSendToPC[18]=tempMusForSend[1];
    IntstringForSendToPC[19]=tempMusForSend[2];
    IntstringForSendToPC[26]=timeMusForSend[0];
	IntstringForSendToPC[27]=timeMusForSend[1];
	IntstringForSendToPC[28]=timeMusForSend[2];
	IntstringForSendToPC[29]=timeMusForSend[3];
	IntstringForSendToPC[30]=timeMusForSend[4];
    IntstringForSendToPC[31] = '8';
}

void SendDataToComputer()
{
	int i = 0;
	for(i=0;i<32;i++)
	{
		StringForSendToPC[i] = IntstringForSendToPC[i];
		while((USART2->SR & USART_SR_TXE) == 0)
		{}
		USART2->DR = StringForSendToPC[i];

	}
}
void ReadStringofDate()		//функция копирования данных в буффер
{
	uint8_t count;
	for(count = 0; count< LenghtTXString; count++)
	{
		buffStringWithCommand[count]=stringWithCommand[count];
	}
}
void getTXString(uint8_t BuffForByte)
{
	if(counterTXGetByte < LenghtTXString-1)
	{
		stringWithCommand[counterTXGetByte] = BuffForByte;
	}
	else if(counterTXGetByte < LenghtTXString)
	{
		stringWithCommand[counterTXGetByte] = BuffForByte;
		flugOfEndCommand = 1;//флаг конца команды
		ReadStringofDate();
	}
	else
	{
		counterTXGetByte = 0;
		stringWithCommand[counterTXGetByte] = BuffForByte;
	}
	counterTXGetByte++;
}
void ReadComandFromBuffer()//функция определения команды из буффера
{
	if((buffStringWithCommand[0] - '0') == FlagStartCommand  && (buffStringWithCommand[31] - '0') == FlagEndCommand)
	{
		numberCommand = (buffStringWithCommand[1] - '0') * 10;
		numberCommand += (buffStringWithCommand[2] - '0');
		commandCheckFlag = CommandIdentified;
	}
	else
	{
		commandCheckFlag = CommandNotIdentified;
		counterTXGetByte = 0;//обнуление счетчика байт если начальный и конечный байт команд не соответствуют
	}
}
void ReadExperimentNumber()
{
	experimentNumber =   (buffStringWithCommand[3] - '0') * 1000;
	experimentNumber +=  (buffStringWithCommand[4] - '0') * 100;
	experimentNumber +=  (buffStringWithCommand[5] - '0') * 10;
	experimentNumber +=  (buffStringWithCommand[6] - '0');
}
void ReadPSCandARRFromBuffer()
{
	TIM2PSC = (buffStringWithCommand[7] - '0') * 10000;
	TIM2PSC += (buffStringWithCommand[8] - '0') * 1000;
	TIM2PSC += (buffStringWithCommand[9] - '0') * 100;
	TIM2PSC += (buffStringWithCommand[10] - '0') * 10;
	TIM2PSC += (buffStringWithCommand[11] - '0');
	TIM2ARR =  (buffStringWithCommand[12] - '0') * 10000;
	TIM2ARR += (buffStringWithCommand[13] - '0') * 1000;
	TIM2ARR += (buffStringWithCommand[14] - '0') * 100;
	TIM2ARR += (buffStringWithCommand[15] - '0') * 10;
	TIM2ARR += (buffStringWithCommand[16] - '0');
}
void ReadTimeFromBuffer()
{
	TimeFromBuffer = (buffStringWithCommand[24] - '0') * 1000000;
	TimeFromBuffer += (buffStringWithCommand[25] - '0') * 100000;
	TimeFromBuffer += (buffStringWithCommand[26] - '0') * 10000;
	TimeFromBuffer += (buffStringWithCommand[27] - '0') * 1000;
	TimeFromBuffer += (buffStringWithCommand[28] - '0') * 100;
	TimeFromBuffer += (buffStringWithCommand[29] - '0') * 10;
	TimeFromBuffer += (buffStringWithCommand[30] - '0');
	SettingHeatingTime(TimeFromBuffer);
}
void ReadTemperatureFromBuffer()
{
	TemperatureFromBuffer = (buffStringWithCommand[17] - '0') * 100;
	TemperatureFromBuffer += (buffStringWithCommand[18] - '0') * 10;
	TemperatureFromBuffer += (buffStringWithCommand[19] - '0');
}
void TemperatureComparison(uint8_t WithTimerOrNot)
{
	if(TemperatureFromBuffer == SensorTemperature)
	{
		if(indicateFirstInput == 1)
		{
		if(WithTimerOrNot == WithTimer)
		{
			StartOneSecondTimer();//после того как температура будет нужной
		}
		IndicationStartExperiment();
		indicateFirstInput=0;
		//добавить индикацию для пользователе  оповещение о начале экспеимента
		}

		flugOfEndCommand = 0;
	}
	else
	{
		if(TemperatureFromBuffer<SensorTemperature)
		{
			TemperatureIsCorrect = TemperatureIsHigher;
		}
		else
		{
			TemperatureIsCorrect = TemperatureIsLower;
		}
		AutoFrequencySetting(TemperatureIsCorrect);
	}
}
void HeatingWithTimer()
{
	ReadExperimentNumber();
	ReadTimeFromBuffer();
	ReadTemperatureFromBuffer();
	StartSignalForHeating();////////////исправить нужно чтобы таймер включался как только будет нужная температура
	TemperatureComparison(WithTimer);
	numberCommand=Temperature_Comparison_WithTimer;
	indicateFirstInput=1;
	PreparCommandToSentComputer();
	SendDataToComputer();
}
void WaitingForCommands()
{
	if(numberCommand != 0)	//проверка пришла ли команды
	{
		state_Control_Device = numberCommand;//если пришла то вызываем соответствующую функцию
	}
}

void HeatingWithoutTimer()
{
		ReadExperimentNumber();
		ReadTemperatureFromBuffer();
		StartSignalForHeating();
		TemperatureComparison(WithoutTimer);
		numberCommand=Temperature_Comparison_WithoutTimer;
		indicateFirstInput = 1;
		PreparCommandToSentComputer();
		SendDataToComputer();
}

void StopHeating()
{
	StopSignalForHeating();
	numberCommand = Stop_Heating;
	PreparCommandToSentComputer();
	SendDataToComputer();
	numberCommand = Waiting_For_Commands;
}

//void SettingTemperature настройка температуры с передачей параметра коефициента

void CommandControl()
{
	switch(numberCommand)
	{
	case Waiting_For_Commands:					WaitingForCommands();				break;
	case Stop_Heating: 							StopHeating(); 						break;
	case Heating_without_timer: 				HeatingWithoutTimer();				break;
	case Heating_with_timer:					HeatingWithTimer();					break;
	case Temperature_Comparison_WithTimer:		TemperatureComparison(WithTimer);	break;
	case Temperature_Comparison_WithoutTimer: 	TemperatureComparison(WithoutTimer);break;
	default: break;
	}
}
