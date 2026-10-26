#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "freertos.h"
#include "task.h"
#include "pwm.h"
#include "key.h"
#include "queue.h"

typedef enum
{
  CMD_PAUSE = 0,
  CMD_SPEED
} cmd_t; // 消息只有两种命令

QueueHandle_t queue9; // ← 全局，放文件顶部！
QueueHandle_t queue7; // 两个呼吸任务各一个队列，否则 key1 的命令
                      // 会被 breath7 抢走（队列是抢收的，不区分收件人）
// PWM呼吸
void task_breath(void *arg)
{
  int id = (int)arg;           // 不再需要 ctx 结构体，arg 直接传通道号
  int pause = 0, step_ms = 10; // ← 状态收回任务自己怀里（这就是"消灭共享"）
  uint16_t compare = 0;
  uint8_t dir = 1;
  cmd_t cmd;
  QueueHandle_t my_queue = (id == 0) ? queue9 : queue7;

  while (1)
  {
    if (xQueueReceive(my_queue, &cmd, 0) == pdTRUE) // ← 在循环里！0=纯查询
    {
      if (cmd == CMD_PAUSE)
        pause = !pause;
      else
        step_ms = (step_ms == 10) ? 2 : 10;
    }
    if (!pause)
    {
      if (dir)
        compare++;
      else
        compare--;
      if (compare > 300)
        dir = 0;
      if (compare == 0)
        dir = 1;
      pwm_set(id, (uint32_t)compare * compare / 300);
    }
    else
    {
      pwm_set(id, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(step_ms));
  }
}

// 短按-开始/暂停呼吸  长按-呼吸快/慢
void task_key(void *arg)
{
  int key_id = (int)arg;
  QueueHandle_t his_queue = (key_id == 0) ? queue9 : queue7;
  while (1)
  {
    key_event_t ev = key_scan(key_id);
    cmd_t cmd;
    if (ev == KEY_SHORT)
    {
      cmd = CMD_PAUSE;
      xQueueSend(his_queue, &cmd, 0);
    }
    if (ev == KEY_LONG)
    {
      cmd = CMD_SPEED;
      xQueueSend(his_queue, &cmd, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

int main(void)
{
  HAL_Init();                         /* 初始化HAL库 */
  sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz */
  delay_init(168);                    /* 延时初始化 */
  usart_init(115200);                 /* 串口初始化为115200 */

  pwm_setup_all();
  key_setup_all();

  queue9 = xQueueCreate(10, sizeof(cmd_t));
  queue7 = xQueueCreate(10, sizeof(cmd_t)); // 两个呼吸任务各一个，两封邮筒

  xTaskCreate(task_breath, "breath9", 256, (void *)0, 2, NULL);
  xTaskCreate(task_breath, "breath7", 256, (void *)1, 2, NULL);
  xTaskCreate(task_key, "key1", 256, (void *)0, 2, NULL);
  xTaskCreate(task_key, "key2", 256, (void *)1, 2, NULL);

  vTaskStartScheduler();

  while (1)
    ;
}
