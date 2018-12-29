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
void Light(int pin);
void Light(int pin)
{
  GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
  int i;
  for (i = 0; i < SWITCH_DELAY; i++);
  switch (pin)
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
  for (i = 0; i < SWITCH_DELAY; i++);
  for (i = 0; i < SWITCH_DELAY; i++);	
  for (i = 0; i < SWITCH_DELAY; i++);	
  for (i = 0; i < SWITCH_DELAY; i++);
  for (i = 0; i < SWITCH_DELAY; i++);	
  for (i = 0; i < SWITCH_DELAY; i++);	
}

int main(void)
{
	LedsInit();
	ButtonsInit();

	int light = 1, order = 0;
	while (1)
	  {
		int a = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0);
		if(a == 0) light *= -1;		  
		order += light;
		order %= 3;
		if(order < 0) order = 2;
		Light(order);
	  }
}
