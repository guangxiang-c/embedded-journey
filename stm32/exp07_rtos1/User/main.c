#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "freertos.h"
#include "task.h"
#include "pwm.h"
#include "key.h"

struct breath_ctx
{
  int id; /* 既是 pwm_set 的通道号，也是 key_scan 的按键号——依赖两表同序的约定！改表时必须一起检查 */
  int pause;
  int step_ms;
};
struct breath_ctx ctx9 = {0, 0, 10};
struct breath_ctx ctx7 = {1, 0, 10};

void task_breath(void *arg)
{
  struct breath_ctx *ctx = (struct breath_ctx *)arg;
  uint16_t compare = 0;
  uint8_t dir = 1;

  while (1)
  {
    if (!ctx->pause)
    {
      if (dir)
        compare++;
      else
        compare--;
      if (compare > 300)
        dir = 0;
      if (compare == 0)
        dir = 1;
      pwm_set(ctx->id, (uint32_t)compare * compare / 300);
    }
    else
    {
      pwm_set(ctx->id, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(ctx->step_ms));
  }
}

void task_key(void *arg)
{
  while (1)
  {
    struct breath_ctx *ctx = (struct breath_ctx *)arg;
    key_event_t ev = key_scan(ctx->id);
    if (ev == KEY_SHORT)
    {
      ctx->pause = !ctx->pause;
    }
    if (ev == KEY_LONG)
    {
      ctx->step_ms = ctx->step_ms == 10 ? 2 : 10;
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

  xTaskCreate(task_breath, "breath9", 256, &ctx9, 2, NULL);
  xTaskCreate(task_breath, "breath7", 256, &ctx7, 2, NULL);

  xTaskCreate(task_key, "key1", 256, &ctx9, 2, NULL);
  xTaskCreate(task_key, "key2", 256, &ctx7, 2, NULL);
  vTaskStartScheduler();

  while (1)
    ;
}
