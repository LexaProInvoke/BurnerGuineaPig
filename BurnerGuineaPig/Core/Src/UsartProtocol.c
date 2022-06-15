#include "UsartProtocol.h"
#include "main.h"

states_Control_Device state_Control_Device = 0;
volatile char stringWithReceivedData[8];
volatile uint8_t FlugOfEndCommand = 0;//���� ����� ����������� �������.
uint8_t StringOfByte[LenghtTXString];
uint8_t BuffStringOfByte[LenghtTXString];
uint8_t counterTXGetByte = 0;
int NumberCommand = 0;

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
		BuffStringOfByte[count]=StringOfByte[count];
	}
}
void getTXString(uint8_t BuffForByte)
{
	if(counterTXGetByte < LenghtTXString-1)
	{
		StringOfByte[counterTXGetByte] = BuffForByte;
	}
	else if(counterTXGetByte < LenghtTXString)
	{
		StringOfByte[counterTXGetByte] = BuffForByte;
		FlugOfEndCommand = 1;//���� ����� �������
		ReadStringofDate();
	}
	else
	{
		counterTXGetByte = 0;
		StringOfByte[counterTXGetByte] = BuffForByte;
	}
	counterTXGetByte++;
}
void getReadComand()//������� ����������� ������� �� �������
{
	if((BuffStringOfByte[0] - '0') == 8  && (BuffStringOfByte[31] - '0') ==8)
	{
		//�urrent�ommand = 0;
		NumberCommand = (BuffStringOfByte[1] - '0') * 10;
		NumberCommand += (BuffStringOfByte[2] - '0');
	}
	else
	{
		counterTXGetByte = 0;//��������� �������� ���� ���� ��������� � �������� ���� ������ �� �������������
	}
}

void WaitingForCommands()
{
	if(NumberCommand != 0)	//�������� ������ �� �������
	{
		state_Control_Device = NumberCommand;//���� ������ �� �������� ��������������� �������
	}
}

void HeatingWithoutTimer()
{
	if(state_Control_Device != Stop_Heating)
	{

	}
}

void StopHeating()
{

}
//void SettingTemperature ��������� ����������� � ��������� ��������� �����������

void CommandControl()
{
	switch(state_Control_Device)
	{
	case Waiting_For_Commands:	WaitingForCommands();	break;
	case Heating_without_timer: HeatingWithoutTimer();	break;
	case Stop_Heating: 			StopHeating(); 			break;
	default: break;
	}
}
