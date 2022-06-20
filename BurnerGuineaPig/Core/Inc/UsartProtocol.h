#include "main.h"
#include "ds18b20.h"
#ifndef INC_USARTPROTOCOL_H_
#define INC_USARTPROTOCOL_H_

#define LenghtTXString			32
#define CommandIdentified 		1
#define CommandNotIdentified 	0
#define FlagStartCommand 		8
#define FlagEndCommand 			8
#define TemperatureCorrect 	    1
#define TemperatureIsHigher     2
#define TemperatureIsLower      3
#define WithTimer				1
#define WithoutTimer			2
extern volatile uint16_t 	experimentNumber;
extern volatile uint8_t 	flugOfEndCommand;
extern volatile uint16_t 	TemperatureFromBuffer;
extern volatile uint8_t 	TemperatureIsCorrect;
extern volatile int 		numberCommand;
extern volatile char 		StringForSendToPC[32];
typedef enum
{
	Waiting_For_Commands = 0,
	Stop_Heating = 1,
	Heating_without_timer = 2,
	Heating_with_timer = 3,
	Temperature_Comparison_WithTimer = 4,
	Temperature_Comparison_WithoutTimer = 5,
} states_Control_Device;
extern states_Control_Device state_Control_Device;
void SendDataToComputer();
void USART_Init();
void ReadComandFromBuffer();
void getTXString(uint8_t BuffForByte);
void CommandControl();
#endif /* INC_USARTPROTOCOL_H_ */
