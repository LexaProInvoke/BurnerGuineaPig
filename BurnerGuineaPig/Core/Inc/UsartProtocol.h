#include "main.h"

#ifndef INC_USARTPROTOCOL_H_
#define INC_USARTPROTOCOL_H_

#define LenghtTXString			32

extern volatile char stringWithReceivedData[8];
extern volatile uint8_t FlugOfEndCommand;

typedef enum
{
	Waiting_For_Commands = 0,
	Stop_Heating = 1,
	Heating_without_timer = 2,
} states_Control_Device;
extern states_Control_Device state_Control_Device;

void USARTADD();
void getReadComand();
void getTXString(uint8_t BuffForByte);
void CommandControl();
#endif /* INC_USARTPROTOCOL_H_ */
