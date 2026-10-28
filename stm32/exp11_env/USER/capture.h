#ifndef _CAPTURE_H
#define _CAPTURE_H
#include "sys.h"

void TIM4_CH1_Cap_Init(u32 arr, u16 psc);
void TIM4_IRQHandler(void);

#endif