#include "led.h"

// LED初始化
void led_init(void)
{
	// 打开GPIOF的时钟
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	// 定义结构体
	GPIO_InitTypeDef GPIO_Initure;
	GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10; // PF7-芯片引脚  PF9-LED0-红灯 PF10-LED1-绿灯
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);
}