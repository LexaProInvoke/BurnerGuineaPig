/*
 * ds18b20.c
 *
 *  Created on: Dec 26, 2021
 *      Author: Asuka
 */

#include "ds18b20.h"

volatile uint32_t timerDelay=0;
volatile float tempMusForSend = 20;
volatile float SensorTemperature = 20;
uint8_t status;
uint8_t dt[8];
uint16_t raw_temper;
char c;

__STATIC_INLINE void DelayMicro(__IO uint32_t micros)
{
	micros *= (SystemCoreClock / 1000000) / 9;

	/* Wait till done */

	while (micros--) ;
//	DelayTime = micros;
//	StartMicrosecondTimer();
}


void PortTemperature_init(void)//настройка порта для отправки данних
{
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);
  GPIOB->OSPEEDR |=0b00001100;
  GPIOB->MODER &= 0xfffffff3;
  GPIOB->MODER |= 0b00000100;
  GPIOB->OTYPER |=0b0010;
}

uint8_t ds18b20_Reset(void)
{
  uint16_t status;
  GPIOB->ODR &= ~0b0010;
  DelayMicro(485);
  GPIOB->ODR |= 0b0010;
  DelayMicro(65);
  status = GPIOB->IDR & 0b0010;
  DelayMicro(200);
  return (status ? 1 : 0);
}

uint8_t ds18b20_ReadBit(void)
{
  uint8_t bit = 0;

  GPIOB->ODR &= ~0b0010;//низкий уровень
  DelayMicro(2);
  GPIOB->ODR |= 0b0010;
    DelayMicro(13);
    bit = ((GPIOB->IDR & 0b0010) ? 1 : 0);
    DelayMicro(45);
      return bit;
}

uint8_t ds18b20_ReadByte(void)
{
  uint8_t data = 0;
  for (uint8_t i = 0; i <= 7; i++)
  data += ds18b20_ReadBit() << i;
  return data;

}
void ds18b20_WriteBit(uint8_t bit)
{
  GPIOB->ODR &= ~0b0010;
  DelayMicro(bit ? 3 : 65);
  GPIOB->ODR |= 0b0010;
  DelayMicro(bit ? 70 : 3);
}
void ds18b20_WriteByte(uint8_t dt)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    ds18b20_WriteBit(dt >> i & 1);
    //Delay Protection
    DelayMicro(5);
  }
}

uint8_t ds18b20_init(uint8_t mode)
{
	if(ds18b20_Reset()) return 1;
	  if(mode==SKIP_ROM)
	  {
		  ds18b20_WriteByte(0xCC);
		  ds18b20_WriteByte(0x4E);
		  ds18b20_WriteByte(0x64);
		  ds18b20_WriteByte(0x9E);
		  ds18b20_WriteByte(RESOLUTION_12BIT);
	  }
  return 0;
}
void ds18b20_MeasureTemperCmd(uint8_t mode, uint8_t DevNum)
{
  ds18b20_Reset();

  if(mode==SKIP_ROM)
  {
    ds18b20_WriteByte(0xCC);
  }
  ds18b20_WriteByte(0x44);

}
void ds18b20_ReadStratcpad(uint8_t mode, uint8_t *Data, uint8_t DevNum)
{
  uint8_t i;
  ds18b20_Reset();
  if(mode==SKIP_ROM)
  {
    ds18b20_WriteByte(0xCC);
  }
  //READ SCRATCHPAD
  ds18b20_WriteByte(0xBE);
  for(i=0;i<8;i++)
  {
    Data[i] = ds18b20_ReadByte();
  }
}

uint8_t ds18b20_GetSign(uint16_t dt)
{

  //Проверим 11-й бит

  if (dt&(1<<11)) return 1;

  else return 0;

}
float ds18b20_Convert(uint16_t dt)

{

  float t;

  t = (float) ((dt&0x07FF)>>4); //отборосим знаковые и дробные биты

  //Прибавим дробную часть

  t += (float)(dt&0x000F) / 16.0f;

  return t;

}

void TemperatureReading()
{	 PortTemperature_init();
	 ds18b20_Reset();
	 status = ds18b20_init(SKIP_ROM);
	 ds18b20_MeasureTemperCmd(SKIP_ROM, 0);
	 HAL_Delay(800);
	 ds18b20_ReadStratcpad(SKIP_ROM, dt, 0);
	 raw_temper = ((uint16_t)dt[1]<<8)|dt[0];
	 if(ds18b20_GetSign(raw_temper)) c='-';
	 else c='+';
	 SensorTemperature = ds18b20_Convert(raw_temper);
}
//--------------------------------------------------
