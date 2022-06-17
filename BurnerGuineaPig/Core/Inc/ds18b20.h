/*
 * ds18b20.h
 *
 *  Created on: Dec 26, 2021
 *      Author: Asuka
 */

#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "stm32f4xx_it.h"
#include "main.h"
#define SKIP_ROM 0
#define NO_SKIP_ROM 1
#define RESOLUTION_9BIT 0x1F
#define RESOLUTION_10BIT 0x3F
#define RESOLUTION_11BIT 0x5F
#define RESOLUTION_12BIT 0x7F

extern volatile uint32_t timerDelay;
extern volatile float SensorTemperature;
void TemperatureReading();
void PortTemperature_init(void);
uint8_t ds18b20_Reset(void);
uint8_t ds18b20_ReadBit(void);
uint8_t ds18b20_ReadByte(void);
void ds18b20_WriteBit(uint8_t bit);
void ds18b20_WriteByte(uint8_t dt);
uint8_t ds18b20_init(uint8_t mode);
void ds18b20_ReadStratcpad(uint8_t mode, uint8_t *Data, uint8_t DevNum);
void ds18b20_MeasureTemperCmd(uint8_t mode, uint8_t DevNum);
float ds18b20_Convert(uint16_t dt);
uint8_t ds18b20_GetSign(uint16_t dt);
float ds18b20_Convert(uint16_t dt);
#endif /* INC_DS18B20_H_ */