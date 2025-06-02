#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"
#include "delay.h"
#include "oled.h"

void TIM3_Int_Init(void);
float Waves_Measure(void);

#endif
