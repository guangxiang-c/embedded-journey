#include "sys.h"
#include "delay.h"
#include "pwm.h"

int main()
{
  int dir = 1;
  u16 compare = 0;
  HAL_Init();                     // 初始化HAL库
  Stm32_Clock_Init(336, 8, 2, 7); // 初始化时钟168Mhz
  delay_init(168);                // 初始化延时函数

  pwm_setup_all();

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
    pwm_set(0, (u32)compare * compare / 300);                 // 亮
    pwm_set(1, (u32)(300 - compare) * (300 - compare) / 300); // 反着走：一灯亮时另一灯灭
    pwm_set(2, (u32)(300 - compare) * (300 - compare) / 300); // 反着走：一灯亮时另一灯灭
  }

  return 0;
}