#ifndef ACTIONS_INDICATION_H
#define ACTIONS_INDICATION_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "player.h"

void Indic_Init(int period, int prescaler);
void SetErrorLight(void);
void SetFixErrorLight(void);
void ResetCommandLight(void);
void SetCommandLight(void);
void SetPauseLight(void);
void SetPlayLight(void);
void TIM3_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI0_IRQHandler(void);

#endif /* ACTIONS_INDICATION_H */