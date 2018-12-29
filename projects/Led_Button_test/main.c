#include "main.h"

#define SWITCH_DELAY 500000

int main(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructureBN;
  /* LEDs array to toggle between them */
  /* LED to toggle during iteration */
  uint8_t  current_led = 0;

  /* Enable peripheral clock for LEDs port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  
  /* Init LEDs */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
 GPIO_InitStructureBN.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructureBN.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructureBN.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructureBN.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructureBN.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructureBN);
GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);

int light=1;
while (1)
 {
	int a =  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0);
	if(a==0) light *= -1;
	if(light==1)
	{
		int i;
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		for (i = 0; i < SWITCH_DELAY; i++);
		GPIO_SetBits(GPIOA,  GPIO_Pin_8);
		for (i = 0; i < SWITCH_DELAY; i++);
	}
	else
	{
		int i;
		GPIO_ResetBits(GPIOA,  GPIO_Pin_10);
		for (i = 0; i < SWITCH_DELAY; i++);
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
		for (i = 0; i < SWITCH_DELAY; i++);
	}
 }
}
 

