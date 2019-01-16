/*!
\file color.c
\brief Library color

Library to set colors
*/
#include "color.h"


#define SWITCH_DELAY 500000
static int period = 30000;

void Init(int per, int pres)
{
	LedsInit();
	Init_TIM1(per, pres);
	Init_PWM();	
}

void LedsInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void SetColor(int red, int green, int blue)
{
	TIM_SetCompare3(TIM1, blue * period / 255);
	TIM_SetCompare1(TIM1, red * period / 255);
	TIM_SetCompare2(TIM1, green * period / 255);
}

void Init_TIM1(int per, int pres)
{
  TIM_TimeBaseInitTypeDef tim_struct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  tim_struct.TIM_Period = per;
  tim_struct.TIM_Prescaler = pres;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &tim_struct);
}

 void Init_PWM(void)
{
  TIM_OCInitTypeDef timerPWM;
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
    timerPWM.TIM_Pulse = 0;
    timerPWM.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM1, &timerPWM);
    TIM_OC2Init(TIM1, &timerPWM);
    TIM_OC3Init(TIM1, &timerPWM);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
} 