#include "sys.h"
#include "delay.h"
#include "timer.h"

int main()
{
  int dir = 1;
  u16 compare = 0;
  HAL_Init();                     // 初始化HAL库
  Stm32_Clock_Init(336, 8, 2, 7); // 初始化时钟168Mhz
  delay_init(168);                // 初始化延时函数

  breath_pwm_init(500 - 1, 84 - 1);
	
  while (1)
  {
    delay_ms(10);
    if (dir)
    {
      compare++;
    }
    else
    {
      compare--;
    }
    if (compare > 300)
      dir = 0;
    if (compare == 0)
      dir = 1;
    __HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, (u32)compare * compare / 300);
		__HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, (u32)compare * compare / 300);
  }

  return 0;
}