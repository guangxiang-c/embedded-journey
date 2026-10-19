#include "key.h"

// key 对应的芯片引脚初始化
void key_init(void)
{
	// 打开GPIOF的时钟
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_Initure;
	GPIO_Initure.Pin = GPIO_PIN_8; // PF8-芯片引脚
	GPIO_Initure.Mode = GPIO_MODE_INPUT; // 模式：输入
	GPIO_Initure.Pull = GPIO_PULLUP; // 上拉：没按时引脚默认高电平
	
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);
}

int key_scan(void)
{
	static uint32_t last_sample = 0;  // 上次采样时刻
	static int stable = 0;            // 已确认的稳定状态
	static int raw = 0;               // 上次原始读数
	static int same_cnt = 0;          // 连续相同次数
	
	uint32_t now = HAL_GetTick();
	// 未到10ms采样周期 -> 立即返回
	if (now - last_sample < 10) {
		return 0;
	}
	last_sample = now;
	
	int current = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8) == GPIO_PIN_RESET); // 按键按下，低电平，结果current为1
	
	if (current == raw) {
		same_cnt++;
	} else {
		raw = current;
		same_cnt = 0;
	}
	
	// 连续3次采样（30ms）结果一致 -> 抖动平息，状态可信
	if(same_cnt >= 3 && current != stable) {
		stable = current;
		if (current == 1) {
			return 1;
		}
	}
	return 0;
}