#include "main.h"

int main(void)
{
	Init_TIM1(30000,100);
	Init_PWM();
	LedsInit();

	while (1)
	  {
		SetColor(102,0,153);
	  }
}
