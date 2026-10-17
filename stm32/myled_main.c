// 双灯交替闪烁 2026-10-17
#include "sys.h"
#include "delay.h"
#include "usart.h"

// LED初始化
void led_init(void)
{
	// 打开GPIOF的时钟
	__HAL_RCC_GPIOF_CLK_ENABLE();

	// 定义结构体
	GPIO_InitTypeDef GPIO_Initure;
	GPIO_Initure.Pin = GPIO_PIN_9 | GPIO_PIN_10; // PF9-LED0-红灯  PF10-LED1-绿灯
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOF, &GPIO_Initure);
}

int main(void)
{
	HAL_Init();						// HAL库初始化
	Stm32_Clock_Init(336, 8, 2, 7); // 时钟配置到168MHZ
	delay_init(168);				// 延时初始化
	led_init();

	int led_state = 0;

	while (1)
	{
		/*HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
		delay_ms(500);

		HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
		delay_ms(500);*/

		if (led_state == 0)
		{
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET); // 红灯亮
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);  // 绿灯灭
			led_state = 1;
		}
		else
		{
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);	   // 红灯灭
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET); // 绿灯亮
			led_state = 0;
		}
		delay_ms(500);
	}
}
