#include "UsartProtocol.h"
#include "ds18b20.h"

states_Control_Device state_Control_Device = 0;
volatile char stringWithReceivedData[8];
volatile uint8_t flugOfEndCommand = 0;//���� ����� ����������� �������.
uint8_t commandCheckFlag = CommandNotIdentified;//or CommandIdentified
uint8_t stringWithCommand[LenghtTXString];
uint8_t buffStringWithCommand[LenghtTXString];
uint8_t counterTXGetByte = 0;
volatile uint8_t TemperatureIsCorrect = TemperatureCorrect;

int numberCommand = 0;
int TIM2ARR = 0;
int TIM2PSC = 0;
uint32_t TimeFromBuffer = 0;
volatile uint16_t TemperatureFromBuffer = 0;
void USARTADD()
{//�������� ������������ UART1. �� ��������� � ���� APB1
	RCC->APB1ENR |= 0b1<<17; // �������� ������������ UART1
	//UART1 ���������� ������: PA2 (��� ������� TX) � PA3 (������ RX). ���� ������ ������������ ��� ���.
	RCC->AHB1ENR |= 0b1;// ��������� ������������ ����� GPIOA

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
	USART2->CR1 |= 0b1000;//���������� �������� ���������
	USART2->CR1 |= 0b100;//���������� �������� ���������
	USART2->CR1 |= 0b100000;//���������� ����������
	USART2->BRR = 4375;//9600 bod �������� �������� ������� ��� �� ������� ��� ������������� �� �������
	USART2->CR2 = 0;
	USART2->CR3 = 0;
	//USART2->
	NVIC_EnableIRQ (USART2_IRQn);//���������� �����w���� �����
}
void ReadStringofDate()		//������� ����������� ������ � ������
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
		flugOfEndCommand = 1;//���� ����� �������
		ReadStringofDate();
	}
	else
	{
		counterTXGetByte = 0;
		stringWithCommand[counterTXGetByte] = BuffForByte;
	}
	counterTXGetByte++;
}
void ReadComandFromBuffer()//������� ����������� ������� �� �������
{
	if((buffStringWithCommand[0] - '0') == FlagStartCommand  && (buffStringWithCommand[31] - '0') == FlagEndCommand)
	{
		numberCommand = (buffStringWithCommand[1] - '0') * 10;
		numberCommand += (buffStringWithCommand[2] - '0');
		commandCheckFlag = CommandIdentified;
	}
	else
	{
		counterTXGetByte = 0;
		commandCheckFlag = CommandNotIdentified;
		counterTXGetByte = 0;//��������� �������� ���� ���� ��������� � �������� ���� ������ �� �������������
	}
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
}

void ReadTemperatureFromBuffer()
{
	TemperatureFromBuffer = (buffStringWithCommand[17] - '0') * 100;
	TemperatureFromBuffer += (buffStringWithCommand[18] - '0') * 10;
	TemperatureFromBuffer += (buffStringWithCommand[19] - '0');
}
void HeatingWithTimer()
{
	//ReadPSCandARRFromBuffer();
	ReadTimeFromBuffer();
	ReadTemperatureFromBuffer();


				StartSignalForHeating();////////////��������� ����� ����� ������ ��������� ��� ������ ����� ������ �����������
				if(TemperatureFromBuffer == SensorTemperature)
				{
					SettingHeatingTime(TimeFromBuffer);//����� ���� ��� ����������� ����� ������
					StartOneSecondTimer();//�������� ��������� ��� ������������  ���������� � ������ �����������
					flugOfEndCommand = 0;
				}
				else
				{
					if(TemperatureFromBuffer>SensorTemperature)
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

void WaitingForCommands()
{
	if(numberCommand != 0)	//�������� ������ �� �������
	{
		state_Control_Device = numberCommand;//���� ������ �� �������� ��������������� �������
	}
}

void HeatingWithoutTimer()
{
//	if(state_Control_Device != Stop_Heating)
//	{
//		ReadPSCandARRFromBuffer();
//		if(SettingFrequencyOutputSignal(TIM2PSC,TIM2ARR) == 1)
//		{
//			StartSignalForHeating();
//		}
//		else
//		{
//			//error send error on computer end displey
//		}
//	}
}

void StopHeating()
{
	StopSignalForHeating();
}
//void SettingTemperature ��������� ����������� � ��������� ��������� �����������

void CommandControl()
{
	switch(numberCommand)
	{
	case Waiting_For_Commands:	WaitingForCommands();	break;
	case Stop_Heating: 			StopHeating(); 			break;
	case Heating_without_timer: HeatingWithoutTimer();	break;
	case Heating_with_timer: HeatingWithTimer();	break;
	default: break;
	}
}
