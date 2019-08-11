#include "actions_indication.h"

void Init_Tim3Inter(void);
void Init_TIM3(uint32_t period, uint32_t prescaler);
void LedsInit(void);
void ButtonsInit(void);
void Init_ButtonInter(void);

void EXTI0_IRQHandler(void) 
{
  if(EXTI_GetITStatus(EXTI_Line0)!= RESET)
  {
    Player_ChangeSong();
    SetCommandLight();
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

void EXTI1_IRQHandler(void) 
{
  if(EXTI_GetITStatus(EXTI_Line1)!= RESET)
  {
    Player_VolumeUp();
    SetCommandLight();
    EXTI_ClearITPendingBit(EXTI_Line1);
  }
}

void EXTI2_IRQHandler(void) 
{
  if(EXTI_GetITStatus(EXTI_Line2)!= RESET)
  {
    Player_VolumeDown();
    SetCommandLight();
    EXTI_ClearITPendingBit(EXTI_Line2);
  }
}

void EXTI3_IRQHandler(void) 
{
  if(EXTI_GetITStatus(EXTI_Line3)!= RESET)
  {
    Player_Toggle();
    TIM_Cmd(TIM5, ENABLE);
    EXTI_ClearITPendingBit(EXTI_Line3);
  }
}

void TIM3_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM3, TIM_IT_Update)!= RESET)
  {		
    ResetCommandLight();
    TIM_Cmd(TIM3, DISABLE);
    TIM_SetCounter(TIM3, 0);
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  }
}

void Indic_Init(uint32_t time_ms)
{
  LedsInit();
  ButtonsInit();
  Init_ButtonInter();  
  Init_TIM3(1400, (uint32_t)time_ms*SYSCLK/28);
  Init_Tim3Inter();
}

void SetPlayLight(void)
{
  GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15);
  GPIO_SetBits(GPIOD, GPIO_Pin_12);
}

void SetPauseLight(void)
{
  GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15);
  GPIO_SetBits(GPIOD, GPIO_Pin_13);
}

void SetCommandLight(void)
{
  GPIO_SetBits(GPIOD, GPIO_Pin_15);
  TIM_Cmd(TIM3, ENABLE);
}

void ResetCommandLight(void)
{
  GPIO_ResetBits(GPIOD, GPIO_Pin_15);
}

void SetErrorLight(void)
{
  GPIO_SetBits(GPIOD, GPIO_Pin_14);
}

void SetFixErrorLight(void)
{
  GPIO_ResetBits(GPIOD, GPIO_Pin_14);
}

void LedsInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

void ButtonsInit(void)
{
  GPIO_InitTypeDef UserButton;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  UserButton.GPIO_Pin   = GPIO_Pin_0;
  UserButton.GPIO_Mode  = GPIO_Mode_IN;
  UserButton.GPIO_OType = GPIO_OType_PP;
  UserButton.GPIO_Speed = GPIO_Speed_100MHz;
  UserButton.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &UserButton);

  GPIO_InitTypeDef GPIO_InitStructureBN;
  GPIO_InitStructureBN.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructureBN.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructureBN.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructureBN.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructureBN.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStructureBN);
}

void Init_ButtonInter(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
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

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource1);
  EXTI_InitStruct.EXTI_Line= EXTI_Line1;
  EXTI_Init(&EXTI_InitStruct);
  nvic_struct_for_EXTI.NVIC_IRQChannel= EXTI1_IRQn;
  NVIC_Init(&nvic_struct_for_EXTI);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource2);
  EXTI_InitStruct.EXTI_Line= EXTI_Line2;
  EXTI_Init(&EXTI_InitStruct);
  nvic_struct_for_EXTI.NVIC_IRQChannel= EXTI2_IRQn;
  NVIC_Init(&nvic_struct_for_EXTI);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource3);
  EXTI_InitStruct.EXTI_Line= EXTI_Line3;
  EXTI_Init(&EXTI_InitStruct);
  nvic_struct_for_EXTI.NVIC_IRQChannel= EXTI3_IRQn;
  NVIC_Init(&nvic_struct_for_EXTI);
}

void Init_TIM3(uint32_t period, uint32_t prescaler)
{
  TIM_TimeBaseInitTypeDef tim_struct;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  tim_struct.TIM_Period = period-1;
  tim_struct.TIM_Prescaler = prescaler-1;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &tim_struct);
  TIM_Cmd(TIM3, DISABLE);
}

void Init_Tim3Inter(void)
{
  NVIC_InitTypeDef nvic_struct;
  nvic_struct.NVIC_IRQChannel = TIM3_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority = 1;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
} 