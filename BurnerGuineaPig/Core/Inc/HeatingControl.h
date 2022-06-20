#include "main.h"

#ifndef INC_HEATINGCONTROL_H_
#define INC_HEATINGCONTROL_H_

#define ErrorTemperatureOverflow 1;//не хватает времени для нагрева до заданной температуры
extern volatile uint16_t PSCFrequencyTimer;
extern volatile uint16_t ARRFrequencyTimer;
extern volatile uint8_t ControllerError;

extern volatile uint8_t offOnDiod;
extern volatile uint32_t countdownHeatingTime;
extern volatile uint32_t DelayTime;

extern volatile uint32_t TimeHighLevel;
extern volatile uint32_t TimeLowLevel;
extern volatile uint32_t TimeHighLevelBuff;
extern volatile uint32_t TimeLowLevelBuff;
//void MicrosecondTimer();
//void StartMicrosecondTimer();
//void StopMicrosecondTimer();

uint8_t SettingFrequencyOutputSignal(uint16_t PSCHighLevel, uint16_t PSCLowLevel, uint16_t ARRHighLevel, uint16_t ARRLowLevel);
void AutoFrequencySetting(uint8_t TemperatureIsCorrect);

void FrequencyTimer_Init();
void StartSignalForHeating();
void StopSignalForHeating();

void SettingHeatingTime(uint32_t NewTime);
void StartOneSecondTimer();
void StopOneSecondTimer();
void OneSecondTimer_Init();

void ConfigGPIO();
#endif /* INC_HEATINGCONTROL_H_ */
