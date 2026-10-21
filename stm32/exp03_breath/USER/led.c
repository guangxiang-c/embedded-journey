#include "led.h"

// LED初始化
void led_init(void)
{
	// 打开GPIOF的时钟
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	// 定义结构体
	GPIO_InitTypeDef GPIO_Initure;
	GPIO_Initure.Pin =GPIO_PIN_9;         // PF9-LED0-红灯
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;  // 模式为AF：服用为定时器控制
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	GPIO_Initure.Alternate = GPIO_AF9_TIM14;
	
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);
}