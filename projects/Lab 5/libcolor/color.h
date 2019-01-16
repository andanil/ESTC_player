/*!
\file color.h
\brief Library color

Library to set colors
*/
#ifndef COLOR_H
#define COLOR_H

#include <stm32f4xx.h>
#include <math.h>
/*!
LED initialization
*/
void LedsInit(void);
/*!
\brief Set color

Target the color set by the user
\param red Value of red component
\param green Value of green component
\param blue Value of blue component
*/
void SetColor(int red, int green, int blue);
/*!
Timer initialization
\param per Period
\param pres Prescaler
*/
void Init_TIM1(int per, int pres);
/*!
PWM initialization
*/
void Init_PWM(void);
/*!
Initialize components
\param per Period
\param pres Prescaler
*/
void Init(int per, int pres);

#endif /*COLOR_H*/