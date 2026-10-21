#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

void breath_pwm_init(u16 arr, u16 psc);
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim11;

#endif