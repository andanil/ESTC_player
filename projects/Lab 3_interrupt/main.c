#include "main.h"

#define SWITCH_DELAY 500000


static void LedsInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
}
static int light = 1;
void EXTI0_IRQHandler(void) 
{
	if(EXTI_GetITStatus(EXTI_Line0)!= RESET)
        {
	 light *= -1;
         EXTI_ClearITPendingBit(EXTI_Line0);
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
}
static void Init_TIM2(int period, int prescaler)
{
  TIM_TimeBaseInitTypeDef tim_struct;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  tim_struct.TIM_Period = period;
  tim_struct.TIM_Prescaler = prescaler;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &tim_struct);
  TIM_Cmd(TIM2, ENABLE);
}

static void Init_Tim2Inter(void)
{
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	NVIC_InitTypeDef nvic_struct;
	nvic_struct.NVIC_IRQChannel = TIM2_IRQn;
	nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_struct.NVIC_IRQChannelSubPriority = 1;
	nvic_struct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_struct);
} 
static void Light(int num)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
	switch (num)
	  {
		case 0 : 
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_8);
			break;
		}
		case 1 : 
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_9);
			break;
		}
		case 2 : 
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_10);
			break;
		}
	  }
}
void TIM2_IRQHandler(void)
{
	static int flag = 0;
	if(TIM_GetITStatus(TIM2, TIM_IT_Update)!= RESET)
	{		
		flag += light;
		flag %= 3;
		if(flag < 0) flag = 2;
		Light(flag);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

int main(void)
{
	LedsInit();
	ButtonsInit();
	Init_TIM2(1400-1, 60000-1);
	Init_Tim2Inter();
	Init_ButtonInter();

	while (1)
	  {
	
	  }
}
