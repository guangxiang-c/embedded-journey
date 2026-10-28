#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "pwm.h"
#include "capture.h"
#include "can.h"
#include "oled.h"
#include <stdio.h>

extern u8 TIM4CH1_CAPTURE_STA;  // 输入捕获状态
extern u32 TIM4CH1_CAPTURE_VAL; // 输入捕获值

int main()
{
  int dir = 1;
  u16 compare = 0;
  char buf[16];

  HAL_Init();                     // 初始化HAL库
  Stm32_Clock_Init(336, 8, 2, 7); // 初始化时钟168Mhz
  delay_init(168);                // 初始化延时函数
  uart_init(115200);              // 初始化 USART

  CAN1_Init();
  oled_i2c_init();
  if (HAL_I2C_IsDeviceReady(&hi2c2, OLED_ADDR, 3, 100) == HAL_OK)
    printf("OLED ACK: 0x3C online\r\n");
  else
    printf("OLED NO ACK: check wiring/power/pullup\r\n");

  if (HAL_I2C_IsDeviceReady(&hi2c2, 0x38 << 1, 3, 100) == HAL_OK)
    printf("AHT20 ACK: 0x38 online\r\n");
  else
    printf("AHT20 NO ACK\r\n");

  oled_init();
  oled_clear();

  // oled_show_char(0, 0, 'A');
  // oled_show_str(0, 0, "HELLO");
  oled_show_str(0, 3, "CAN OK");

  pwm_setup_all();
  TIM4_CH1_Cap_Init(0XFFFFFFFF, 84 - 1); // 以 1MHZ 的频率计数

  while (1)
  {

    delay_ms(10);

    // 任务1：呼吸灯
    if (dir)
      compare++;
    else
      compare--;
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

    // 任务2：OLED
    uint32_t now = HAL_GetTick();
    static uint32_t last_oled = 0;
    if (now - last_oled >= 100)
    {
      last_oled = now;
      sprintf(buf, "CMP:%3d", compare);
      oled_show_str(0, 2, buf);
    }

    // 任务3：ATH20 -- 每1秒采一次
    static uint32_t last_aht = 0;
    if (now - last_aht >= 1000)
    {
      last_aht = now;
      uint8_t cmd[3] = {0xAC, 0x33, 0x00};
      HAL_I2C_Master_Transmit(&hi2c2, 0x38 << 1, cmd, 3, 100);
      delay_ms(80); // 等它测完（手册说 75ms+）

      uint8_t data[6] = {0};
      if (HAL_I2C_Master_Receive(&hi2c2, 0x38 << 1, data, 6, 100) == HAL_OK)
      {
        if ((data[0] & 0x80) == 0) // bit7=0 表示测量完成
        {
          uint32_t humi = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
          uint32_t temp = (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];

          int humi_x10 = (humi * 1000) / 1048576;         // 定点：0.1%
          int temp_x10 = ((temp * 2000) / 1048576) - 500; // 定点：0.1℃

          printf("T:%d.%dC  H:%d.%d%%\r\n",
                 temp_x10 / 10, temp_x10 % 10,
                 humi_x10 / 10, humi_x10 % 10);
          // 把 printf 那两行换成：
          sprintf(buf, "T:%2d.%dC", temp_x10 / 10, temp_x10 % 10);
          oled_show_str(0, 0, buf);
          sprintf(buf, "H:%2d.%d%%", humi_x10 / 10, humi_x10 % 10);
          oled_show_str(0, 1, buf);

          // 自定义协议：ID 0x131 = 环境数据帧
          // data[0..1] = 温度×10（大端）  253 = 25.3℃
          // data[2..3] = 湿度×10（大端）  582 = 58.2%
          // data[4]    = 帧序号（看丢帧）
          static uint8_t seq = 0;
          uint8_t frame[8] = {0};
          frame[0] = temp_x10 >> 8;
          frame[1] = temp_x10 & 0xFF;
          frame[2] = humi_x10 >> 8;
          frame[3] = humi_x10 & 0xFF;
          frame[4] = seq++;
          uint8_t ok = CAN1_Send(0x131, frame, 8);
          printf("TX ok=%d\r\n", ok);
        }
      }

      uint32_t id;
      uint8_t rx[8];
      uint8_t len = CAN1_Receive(&id, rx);
      if (len)
        printf("Rx ID:%x len:%d  data:%d %d %d %d\r\n", id, len, rx[0], rx[1], rx[2], rx[3]);
    }
  }

  return 0;
}