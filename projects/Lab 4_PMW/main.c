#include "main.h"

#define SWITCH_DELAY 500000


static void LedsInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin   =GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
static int light = 10000;
void EXTI0_IRQHandler(void) 
{
	if(EXTI_GetITStatus(EXTI_Line0)!= RESET)
        {
	 if(light += 1500 > 30000)light = 30000;
         TIM_SetCompare3(TIM1, light);
         EXTI_ClearITPendingBit(EXTI_Line0);
        }
}
void EXTI1_IRQHandler(void) 
{
	if(EXTI_GetITStatus(EXTI_Line1)!= RESET)
        {
	 if(light -= 1500 < 0)light=0;
         TIM_SetCompare3(TIM1, light);
         EXTI_ClearITPendingBit(EXTI_Line1);
        }
}

static void ButtonsInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructureBN;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  GPIO_InitStructureBN.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructureBN.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructureBN.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructureBN.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructureBN.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructureBN);
}
static void Init_ButtonInter(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource0);
  EXTI_InitTypeDef EXTI_InitStruct;
  EXTI_InitStruct.EXTI_Line= EXTI_Line0;
  EXTI_InitStruct.EXTI_LineCmd= ENABLE;
  EXTI_InitStruct.EXTI_Mode= EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger= EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStruct);
  NVIC_InitTypeDef nvic_struct_for_EXTI;
  nvic_struct_for_EXTI.NVIC_IRQChannel= EXTI0_IRQn;
  nvic_struct_for_EXTI.NVIC_IRQChannelPreemptionPriority= 0;
  nvic_struct_for_EXTI.NVIC_IRQChannelSubPriority= 0;
  nvic_struct_for_EXTI.NVIC_IRQChannelCmd= ENABLE;
  NVIC_Init(&nvic_struct_for_EXTI);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource1);
  EXTI_InitTypeDef EXTI_InitStruct1;
  EXTI_InitStruct1.EXTI_Line= EXTI_Line1;
  EXTI_InitStruct1.EXTI_LineCmd= ENABLE;
  EXTI_InitStruct1.EXTI_Mode= EXTI_Mode_Interrupt;
  EXTI_InitStruct1.EXTI_Trigger= EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStruct1);
  NVIC_InitTypeDef nvic_struct_for_EXTI1;
  nvic_struct_for_EXTI1.NVIC_IRQChannel= EXTI1_IRQn;
  nvic_struct_for_EXTI1.NVIC_IRQChannelPreemptionPriority= 0;
  nvic_struct_for_EXTI1.NVIC_IRQChannelSubPriority= 0;
  nvic_struct_for_EXTI1.NVIC_IRQChannelCmd= ENABLE;
  NVIC_Init(&nvic_struct_for_EXTI1);
}
static void Init_TIM1(int period, int prescaler)
{
  TIM_TimeBaseInitTypeDef tim_struct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  tim_struct.TIM_Period = period;
  tim_struct.TIM_Prescaler = prescaler;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &tim_struct);
}

static void Init_PWM(int pulse)
{
  TIM_OCInitTypeDef timerPWM;
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
    timerPWM.TIM_Pulse = pulse;
    timerPWM.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC3Init(TIM1, &timerPWM);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
} 

int main(void)
{

	ButtonsInit();
	Init_TIM1(30000,100);
	Init_PWM(15000);
	Init_ButtonInter();
	LedsInit();

	while (1)
	  {
	
	  }
}
