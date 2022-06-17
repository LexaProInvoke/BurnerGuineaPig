#include "main.h"
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
extern volatile char stringWithReceivedData[8];
extern volatile uint8_t flugOfEndCommand;
extern volatile uint16_t TemperatureFromBuffer;
extern volatile uint8_t TemperatureIsCorrect;
typedef enum
{
	Waiting_For_Commands = 0,
	Stop_Heating = 1,
	Heating_without_timer = 2,
	Heating_with_timer = 3,
} states_Control_Device;
extern states_Control_Device state_Control_Device;

void USARTADD();
void ReadComandFromBuffer();
void getTXString(uint8_t BuffForByte);
void CommandControl();
#endif /* INC_USARTPROTOCOL_H_ */
