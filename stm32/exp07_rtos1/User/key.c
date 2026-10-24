#include "key.h"

static void gpiof_clk_on(void) { __HAL_RCC_GPIOF_CLK_ENABLE(); };
static void gpioe_clk_on(void) { __HAL_RCC_GPIOE_CLK_ENABLE(); };

static struct key_instance key_table[] = {
	{GPIOF, GPIO_PIN_8, gpiof_clk_on, 0, 0, 0, 0, 0, 0},
	{GPIOE, GPIO_PIN_2, gpioe_clk_on, 0, 0, 0, 0, 0, 0}};

// key 对应的芯片引脚初始化
void key_setup_all(void)
{
	int len = sizeof(key_table) / sizeof(key_table[0]);
	for (int i = 0; i < len; i++)
	{
		// 打开GPIOF的时钟
		key_table[i].clk_on();

		GPIO_InitTypeDef GPIO_Initure;
		GPIO_Initure.Pin = key_table[i].pin; // 芯片引脚
		GPIO_Initure.Mode = GPIO_MODE_INPUT; // 模式：输入
		GPIO_Initure.Pull = GPIO_PULLUP;	 // 上拉：没按时引脚默认高电平

		HAL_GPIO_Init(key_table[i].port, &GPIO_Initure);
	}
}

key_event_t key_scan(int id)
{
	uint32_t now = HAL_GetTick();
	// 未到10ms采样周期 -> 立即返回
	if (now - key_table[id].last_sample < KEY_SAMPLE_MS)
	{
		return KEY_NONE;
	}
	key_table[id].last_sample = now;

	int current = (HAL_GPIO_ReadPin(key_table[id].port, key_table[id].pin) == GPIO_PIN_RESET); // 按键按下，低电平，结果current为1

	if (current == key_table[id].raw)
	{
		key_table[id].same_cnt++;
	}
	else
	{
		key_table[id].raw = current;
		key_table[id].same_cnt = 0;
	}

	// 连续3次采样（30ms）结果一致 -> 抖动平息，状态可信
	if (key_table[id].same_cnt >= KEY_DEBOUNCE_CNT && current != key_table[id].stable)
	{
		key_table[id].stable = current;
		// 稳定状态0->1（刚按下）
		if (current == 1)
		{
			key_table[id].press_time = now; // 记录按下时刻的时间戳
			key_table[id].long_reported = 0;
			return KEY_NONE;
		}
		// 稳定状态1->0（松手了）
		else
		{
			if (key_table[id].long_reported == 0)
			{
				return KEY_SHORT;
			}
			return KEY_NONE;
		}
	}

	if (key_table[id].stable == 1 && key_table[id].long_reported == 0 && now - key_table[id].press_time >= KEY_LONG_PRESS_MS)
	{
		key_table[id].long_reported = 1;
		return KEY_LONG;
	}

	return KEY_NONE;
}