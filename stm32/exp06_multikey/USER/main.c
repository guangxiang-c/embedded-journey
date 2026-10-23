#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "pwm.h"
#include "capture.h"
#include "key.h"

extern u8 TIM4CH1_CAPTURE_STA;  // 输入捕获状态
extern u32 TIM4CH1_CAPTURE_VAL; // 输入捕获值

// 每盏灯一份（三个实例）
struct breath_inst
{
  u16 compare;
  u8 dir;
  u32 last_step;
};
void breath_update(struct breath_inst *b, int pwm_id, int step_ms);

int main()
{
  int pf9_pause = 0;
  int pf7_pause = 0;
  int pf9_step_ms = 10;
  int pf7_step_ms = 10;
  int pf6_step_ms = 10;
  struct breath_inst breath9 = {0, 1, 0}; // compare=0, dir=1, last_step=0
  struct breath_inst breath7 = {0, 1, 0};
  struct breath_inst breath6 = {0, 1, 0};

  HAL_Init();                     // 初始化HAL库
  Stm32_Clock_Init(336, 8, 2, 7); // 初始化时钟168Mhz
  delay_init(168);                // 初始化延时函数
  uart_init(115200);              // 初始化 USART

  pwm_setup_all();
  TIM4_CH1_Cap_Init(0XFFFFFFFF, 84 - 1); // 以 1MHZ 的频率计数

  key_setup_all();

  while (1)
  {
    key_event_t key1_command = key_scan(0); // PF8-KEY1
    key_event_t key2_command = key_scan(1); // PE2-KEY2

    // 短按KEY1切PF9呼吸暂停/恢复
    if (key1_command == KEY_SHORT)
    {
      pf9_pause = !pf9_pause;
    }
    else if (key1_command == KEY_LONG)
    {
      pf9_step_ms = pf9_step_ms == 10 ? 5 : 10;
    }
    // 短按KEY2切PF7呼吸暂停/恢复
    if (key2_command == KEY_SHORT)
    {
      pf7_pause = !pf7_pause;
    }
    else if (key2_command == KEY_LONG)
    {
      pf7_step_ms = pf7_step_ms == 10 ? 5 : 10;
    }

    // main 的 while 变成三行平行调用，谁也不用等谁：
    if (!pf9_pause)
    {
      breath_update(&breath9, 0, pf9_step_ms);
    }
    else
    {
      pwm_set(0, 0); // 暂停 = 熄灭，恢复后从断点继续呼吸
    }
    if (!pf7_pause)
    {

      breath_update(&breath7, 1, pf7_step_ms);
    }
    else
    {
      pwm_set(1, 0); // 暂停 = 熄灭，恢复后从断点继续呼吸
    }
    breath_update(&breath6, 2, pf6_step_ms);

    if (TIM4CH1_CAPTURE_STA & 0X80) // 成功捕获到了一次高电平
    {
      if (TIM4CH1_CAPTURE_VAL > 50 && TIM4CH1_CAPTURE_VAL < 100000) // 合理范围守卫
      {
        // printf("period=%u us, freq=%lu Hz\r\n", TIM4CH1_CAPTURE_VAL, 1000000UL / TIM4CH1_CAPTURE_VAL);
      }
      // 不合理 → 丢弃这帧，照样清标志开始下一轮

      TIM4CH1_CAPTURE_STA = 0; // 开启下一次捕获
    }
  }

  return 0;
}

void breath_update(struct breath_inst *b, int pwm_id, int step_ms)
{
  u32 now = HAL_GetTick();
  if (now - b->last_step < step_ms)
    return; // 没到点，各走各的路
  b->last_step = now;

  if (b->dir)
    b->compare++;
  else
    b->compare--;
  if (b->compare > 300)
    b->dir = 0;
  if (b->compare == 0)
    b->dir = 1;
  pwm_set(pwm_id, (u32)b->compare * b->compare / 300);
}
