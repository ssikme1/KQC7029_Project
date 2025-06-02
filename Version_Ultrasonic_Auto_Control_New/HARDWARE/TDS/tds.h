#ifndef __WATER_H
#define	__WATER_H

#include "sys.h"
#include "adcx.h"
#include "delay.h"
#include "math.h"

#define TDS_READ_TIMES	10  //Number of TDS sensor ADC cycle readings

// TDS GPIO Macro Definitions
#define		TDS_GPIO_CLK								RCC_APB2Periph_GPIOA
#define 	TDS_GPIO_PORT								GPIOA
#define 	TDS_GPIO_PIN								GPIO_Pin_0
#define   ADC_CHANNEL									ADC_Channel_0	// ADC Channel Macro Definitions

void TDS_Init(void);
uint16_t TDS_GetData(void);
float TDS_GetData_PPM(void);
float GH_GetData_DH(float TDS);

#endif
