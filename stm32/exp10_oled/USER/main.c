#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "pwm.h"
#include "capture.h"
#include "oled.h"

extern u8 TIM4CH1_CAPTURE_STA;  // 输入捕获状态
extern u32 TIM4CH1_CAPTURE_VAL; // 输入捕获值

int main()
{
  int dir = 1;
  u16 compare = 0;

  HAL_Init();                     // 初始化HAL库
  Stm32_Clock_Init(336, 8, 2, 7); // 初始化时钟168Mhz
  delay_init(168);                // 初始化延时函数
  uart_init(115200);              // 初始化 USART

  oled_i2c_init();
  //if (HAL_I2C_IsDeviceReady(&hi2c2, OLED_ADDR, 3, 100) == HAL_OK)
  //  printf("OLED ACK: 0x3C online\r\n");
  //else
  //  printf("OLED NO ACK: check wiring/power/pullup\r\n");

  oled_init();
  oled_clear();

  pwm_setup_all();
  TIM4_CH1_Cap_Init(0XFFFFFFFF, 84 - 1); // 以 1MHZ 的频率计数

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

    if (TIM4CH1_CAPTURE_STA & 0X80) // 成功捕获到了一次高电平
    {
      if (TIM4CH1_CAPTURE_VAL > 50 && TIM4CH1_CAPTURE_VAL < 100000) // 合理范围守卫
      {
        printf("period=%u us, freq=%lu Hz\r\n", TIM4CH1_CAPTURE_VAL, 1000000UL / TIM4CH1_CAPTURE_VAL);
      }
      // 不合理 → 丢弃这帧，照样清标志开始下一轮

      TIM4CH1_CAPTURE_STA = 0; // 开启下一次捕获
    }
  }

  return 0;
}