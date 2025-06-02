#include "relay.h"

/**
 * @brief Relay initialization function.
 *        This function initializes the GPIO ports and pins for controlling Pump1 and Pump2.
 *        Pump1 is responsible for pumping water out, and Pump2 is responsible for adding water in.
 * @port  Pump1: GPIO_A_11    Pump2: GPIO_A_12
 */
void Relay_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    // Enable GPIOA clock
    
    GPIO_InitTypeDef GPIO_InitStructure;                     // Initialize PA11 & PA12
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);    // Turn off Pump1
    GPIO_ResetBits(GPIOA, GPIO_Pin_12);    // Turn off Pump2
}

// starts pumping water out
void Pump_1_ON(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_11);      // Set pin to High
}

// stops pumping water out
void Pump_1_OFF(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_11);    // Set pin to Low
}

// starts adding water in
void Pump_2_ON(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_12);      // Set pin to High
}

// stops adding water in
void Pump_2_OFF(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_12);    // Set pin to Low
}

/**
 * @brief  Function to pump water out using Pump1
 *         Continuously measures water level and stops Pump1 when the level exceeds 12.5cm or when Key3 is pressed
 * @return Returns 0 and turn back to main loop
 */
int Pump_Out(void)
{
    Pump_1_ON();     // Turn on Pump1
    Pump_2_OFF();    // Ensure Pump2 is off
    
    while(1)
    {
        dis = Waves_Measure();                              // Measure water level
        gh_data = GH_GetData_DH(TDS_GetData_PPM());         // Calculate GH value based on TDS
        OLED_ShowFloatNum(88,16,dis,2,1,OLED_8X16);         // Display water level on OLED
        OLED_ShowFloatNum(72,32,gh_data,2,1,OLED_8X16);     // Display GH value on OLED
        OLED_ShowString(0,48,"Pump1 Working",OLED_8X16);    // Indicate Pump1 status
        OLED_Update();                                      // Refresh OLED display
        if(dis>=12.5)            // Stop Pump1 if water level exceeds 12.5 cm
        {
            Pump_1_OFF();
            return 0;
        }
        
        if(Key_GetNum() == 3)    // Stop Pump1 if Key3 is pressed
		{
			Pump_1_OFF();
            return 0;
		}
    }
}

/**
 * @brief  Function to add water using Pump2
 *         Continuously measures water level and stops Pump2 when the level drops below 5cm
 *         and an infrared sensor detects a specific condition, or when Key3 is pressed
 * @return Returns 0 and turn back to main loop
 */
int Pump_In(void)
{
    Pump_2_ON();     // Turn on Pump2
    Pump_1_OFF();    // Ensure Pump1 is off
    
    uint8_t i=0;
    
    while(1)
    {
        dis = Waves_Measure();
        gh_data = GH_GetData_DH(TDS_GetData_PPM());
        OLED_ShowFloatNum(88,16,dis,2,1,OLED_8X16);
        OLED_ShowFloatNum(72,32,gh_data,2,1,OLED_8X16);
        OLED_ShowString(0,48,"Pump2 Working",OLED_8X16);    // Indicate Pump2 status
        OLED_Update();
        if(dis<5)    // Increment counter if water level is below 5cm
        {
            i+=1;
        }
        
        if(i==10 && Sensor_Get() == 1)    // Stop Pump2 if water level is stable and sensor condition is met
        {
            Pump_2_OFF();
            i%=10;    // Reset counter
            return 0;
        }
        
        if(Key_GetNum() == 3)    // Stop Pump2 if Key3 is pressed
		{
            Pump_2_OFF();
            return 0;
		}
    }
}

/**
 * @brief Key initialization function.
 * @port Key1: GPIO_A_6    Key2: GPIO_A_7    Key3: GPIO_A_5
 */
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    // Enable GPIOA clock
	
	GPIO_InitTypeDef GPIO_InitStructure;                     // Initialize PA5 & PA6 & PA7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// Reads the state of the keys and returns the key number
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum=0;
	
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
	{
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0);
		delay_ms(20);
		KeyNum = 1;
	}
	
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0)
	{
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0);
		delay_ms(20);
		KeyNum = 2;
	}
    
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)
	{
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0);
		delay_ms(20);
		KeyNum = 3;
	}
	
	return KeyNum;
}

// Infrared sensor initialization function
void Sensor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    // Enable GPIOA clock
	
	GPIO_InitTypeDef GPIO_InitStructure;                     // Initialize PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_1);                         // Set default state
}

// Reads the state of the infrared sensor
uint8_t Sensor_Get(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
}
