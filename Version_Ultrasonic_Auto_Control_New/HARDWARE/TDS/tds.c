#include "tds.h"

/**
 * @brief Initializes the TDS sensor
 * @port  GPIO_A_0
 */
void TDS_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd (TDS_GPIO_CLK, ENABLE);	   
    GPIO_InitStructure.GPIO_Pin = TDS_GPIO_PIN;		   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	   // Set to analog input
    
    GPIO_Init(TDS_GPIO_PORT, &GPIO_InitStructure);	   // Initialize ADC IO

    ADCx_Init();                                       
}

/**
 * @brief  Reads the raw ADC value from the TDS sensor
 * @return The raw ADC value (0-4095 for a 12-bit ADC)
 */
uint16_t TDS_ADC_Read(void)
{
	// Sets the rule group channel of the specified ADC, sampling time
	return ADC_GetValue(ADC_CHANNEL, ADC_SampleTime_55Cycles5);
}

/**
 * @brief Reads multiple ADC samples from the TDS sensor and calculates the average
 * @return The averaged raw ADC value
 */
uint16_t TDS_GetData(void)
{
	uint32_t  tempData = 0;
    
    // Perform multiple ADC readings and accumulate their values
	for (uint8_t i = 0; i < TDS_READ_TIMES; i++)
	{
		tempData += TDS_ADC_Read();    // Read ADC value
		delay_ms(5);                   // Short delay between readings
	}

	tempData /= TDS_READ_TIMES;        // Calculate the average
	return tempData;
}

/**
 * @brief Calculates the TDS value in ppm from the sensor's raw ADC readings
 * @return TDS value in ppm
 */
float TDS_GetData_PPM(void)
{
	float  tempData = 0;
	float TDS_DAT;

	for (uint8_t i = 0; i < TDS_READ_TIMES; i++)
	{
		tempData += TDS_ADC_Read();    // Read ADC value
		delay_ms(5);
	}
	tempData /= TDS_READ_TIMES;        // Calculate the average ADC value
	
    // Convert the ADC value to voltage
	TDS_DAT = (tempData/4095.0)*5.0;
    
    // Apply a polynomial curve to calculate the TDS value in ppm
	TDS_DAT = 66.71*TDS_DAT*TDS_DAT*TDS_DAT-127.93*TDS_DAT*TDS_DAT+428.7*TDS_DAT;
    
    // Ensure the TDS value is zero if it falls below the noise threshold
	if(TDS_DAT<20) TDS_DAT = 0;
	
	return TDS_DAT;
}

/**
 * @brief Calculates the GH value in dGH from the TDS value
 * @param TDS value in ppm
 * @return GH value in dGH
 */
float GH_GetData_DH(float TDS)
{
	float DH_DAT;
    
    DH_DAT = TDS/18.75;    // Convert TDS to GH using the conversion factor
	
	return DH_DAT;
}
