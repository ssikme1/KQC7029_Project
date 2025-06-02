#ifndef __RELAY_H
#define __RELAY_H

#include "sys.h"
#include "delay.h"
#include "timer.h"
#include "OLED.h"
#include "tds.h"

extern float dis;
extern float gh_data;
extern uint8_t KeyNum;

void Relay_Init(void);
void Pump_1_ON(void);
void Pump_1_OFF(void);
void Pump_2_ON(void);
void Pump_2_OFF(void);
int Pump_Out(void);
int Pump_In(void);
void Key_Init(void);
uint8_t Key_GetNum(void);
void Sensor_Init(void);
uint8_t Sensor_Get(void);

#endif
