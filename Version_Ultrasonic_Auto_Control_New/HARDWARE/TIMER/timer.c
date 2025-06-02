#include "timer.h"

/**
 * @brief Ultrosonic Sensor Initialize and Measure distance
 * @port  TRIG: GPIO_B_0    ECHO: GPIO_B_1 
 */

unsigned overcount=0;    // Record the number of timer overflows

/* Initialize */
void TIM3_Int_Init(void)
{
    // Initialize Port
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;
    GPIO_Init(GPIOB,&GPIO_InitStruct);

    // Timer TIM3 initialization
    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler =71;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);    // Enable the specified TIM3 interrupt and allow update interrupt

    // Interrupt Priority NVIC Setting
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);

    TIM_Cmd(TIM3, DISABLE);    // Enable TIM3
}

/* Measure distance */
float Waves_Measure(void)
{
    float length=0,sum=0;
    u16 tim;
    unsigned int i=0;
    // Take five measurements and calculate an average
    while(i!=5)
    {
        PBout(0)=1;                                             // Pull-up signal, as a trigger signal
        delay_us(20);                                           // High level signal over 10us
        PBout(0)=0;                                             // Wait for the echo signal
        while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==RESET);
        TIM_Cmd(TIM3,ENABLE);                                   // Echo signal arrives, start timer counting
        i+=1;
        while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==SET);    // Reverberation signal disappears
        TIM_Cmd(TIM3,DISABLE);                                  // Turn off the timer
        tim=TIM_GetCounter(TIM3);                               // Get the count value in the TIM3 count register
        length=(tim+overcount*1000)/58.0;                       // Distance calculation by echo signal
        sum=length+sum;
        TIM3->CNT=0;                                            // Clear the count value of the TIM3 count register to zero
        overcount=0;                                            // Zero interrupt overflow count
        delay_ms(100);
    }
    length=sum/5;                                               // Distance as function return value
    return length;
}

/* Timer 3 interrupt service program */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)    // Check if TIM3 update interrupt occurred or not
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);      // Clear the TIM3 update interrupt flag
        overcount++;
    }
}
