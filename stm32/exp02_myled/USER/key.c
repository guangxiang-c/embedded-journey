#include "key.h"

// key 对应的芯片引脚初始化
void key_init(void)
{
	// 打开GPIOF的时钟
	__HAL_RCC_GPIOF_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_Initure;
	GPIO_Initure.Pin = GPIO_PIN_8;		 // PF8-芯片引脚
	GPIO_Initure.Mode = GPIO_MODE_INPUT; // 模式：输入
	GPIO_Initure.Pull = GPIO_PULLUP;	 // 上拉：没按时引脚默认高电平

	HAL_GPIO_Init(GPIOF, &GPIO_Initure);
}

key_event_t key_scan(void)
{
	static uint32_t last_sample = 0; // 上次采样时刻
	static int stable = 0;			 // 已确认的稳定状态
	static int raw = 0;				 // 上次原始读数
	static int same_cnt = 0;		 // 连续相同次数
	static uint32_t press_time = 0;	 // 按下时刻时间戳
	static int long_reported = 0;	 // 本次按住是否已经上报过长按

	uint32_t now = HAL_GetTick();
	// 未到10ms采样周期 -> 立即返回
	if (now - last_sample < KEY_SAMPLE_MS)
	{
		return KEY_NONE;
	}
	last_sample = now;

	int current = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8) == GPIO_PIN_RESET); // 按键按下，低电平，结果current为1

	if (current == raw)
	{
		same_cnt++;
	}
	else
	{
		raw = current;
		same_cnt = 0;
	}

	// 连续3次采样（30ms）结果一致 -> 抖动平息，状态可信
	if (same_cnt >= KEY_DEBOUNCE_CNT && current != stable)
	{
		stable = current;
		// 稳定状态0->1（刚按下）
		if (current == 1)
		{
			press_time = now; // 记录按下时刻的时间戳
			long_reported = 0;
			return KEY_NONE;
		}
		// 稳定状态1->0（松手了）
		else
		{
			if (long_reported == 0)
			{
				return KEY_SHORT;
			}
			return KEY_NONE;
		}
	}

	if (stable == 1 && long_reported == 0 && now - press_time >= KEY_LONG_PRESS_MS)
	{
		long_reported = 1;
		return KEY_LONG;
	}

	return KEY_NONE;
}