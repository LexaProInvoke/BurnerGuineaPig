#include "main.h"

#ifndef INC_HEATINGCONTROL_H_
#define INC_HEATINGCONTROL_H_

#define ErrorInFrequencySelection 1;
extern volatile uint8_t ControllerError;

extern volatile uint8_t offOnDiod;
extern volatile uint32_t countdownHeatingTime;
extern volatile uint32_t DelayTime;

extern volatile uint16_t ARRHighLevelSignal;
extern volatile uint16_t ARRLowLevelSignal;
extern volatile uint16_t PSCHighLevelSignal;
extern volatile uint16_t PSCLowLevelSignal;

//void MicrosecondTimer();
//void StartMicrosecondTimer();
//void StopMicrosecondTimer();

uint8_t SettingFrequencyOutputSignal(uint16_t PSCHighLevel, uint16_t PSCLowLevel, uint16_t ARRHighLevel, uint16_t ARRLowLevel);


void FrequencyTimerSetting();
void StartSignalForHeating();
void StopSignalForHeating();

void SettingHeatingTime(uint32_t NewTime);
void StartOneSecondTimer();
void StopOneSecondTimer();
void OneSecondTimerSetting();

void configGPIO();
#endif /* INC_HEATINGCONTROL_H_ */
