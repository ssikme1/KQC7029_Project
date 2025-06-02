#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "OLED.h"
#include "tds.h"
#include "adcx.h"
#include "relay.h"

/* Data variable declarations */
float dis;         // distance value
float gh_data;     // gh value
uint8_t KeyNum;    // receive key return value

/* main function */
int main()
{
    /* Initialization Part */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    // interrupt grouping
    delay_init();                                      // delay function
    USART2_Init(115200);                               // USART
    TIM3_Int_Init();                                   // Timer for Ultrasonic Sensor
    OLED_Init();                                       // OLED for Display
    TDS_Init();                                        // TDS module
    Relay_Init();                                      // relay
    Key_Init();                                        // button
    Sensor_Init();                                     // infrared light gate
    
    OLED_ShowString(0,0,"    Aquarium", OLED_8X16);    // Initialization of OLED display
    OLED_ShowString(0,16,"WaterLevel:", OLED_8X16);
    OLED_ShowString(0,32,"GH Level:", OLED_8X16);
    OLED_ShowString(0,48,"Pump Resting ",OLED_8X16);
    OLED_Update();
    /* End Initialization */
    
    /* Main Loop */
    while(1)
    {
        dis = Waves_Measure();                             // Read distance value
        gh_data = GH_GetData_DH(TDS_GetData_PPM());        // Read gh value
        
        OLED_ShowFloatNum(88,16,dis,2,1,OLED_8X16);        // Display the distance value in OLED with one decimal place reservation
        OLED_ShowFloatNum(72,32,gh_data,2,1,OLED_8X16);    // Display the gh value in OLED with one decimal place reservation
        OLED_ShowString(0,48,"Pump Resting ",OLED_8X16);   // Displays the rest state when the pumps is not in operation
        OLED_Update();                                     // refresh the OLED
        
        KeyNum = Key_GetNum();                             // Read key status
        
		if(KeyNum == 1)                                    // If button1 is pressed, start pump1 to pump water 
		{
			Pump_Out();
		}
		if(KeyNum == 2)                                    // If button2 is pressed, start pump2 to add water
		{
			Pump_In();
		}
        
        if(gh_data > 12)                                   // If the gh is greater than 12, activate the automatic water change 
        {
            Pump_Out();
            Pump_In();
        }
    }
    /* End Loop */
}
