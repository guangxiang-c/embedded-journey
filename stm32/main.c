#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"

int main(void)
{
	HAL_Init(); // HAL库初始化
	Stm32_Clock_Init(336,8,2,7); // 时钟配置到168MHZ
	delay_init(168); // 延时初始化
	led_init();
	key_init();
	uint32_t last_blink = 0; // 上次翻灯时刻
	
	while (1) {
		uint32_t now = HAL_GetTick(); // 现在几点（毫秒）
		
		
		// 按键控制绿灯
		if (key_scan()) {
			HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
		}
		
		// 双红灯闪
		if(now - last_blink >= 500){
			last_blink = now;
			HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_7);
			HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
		}
	}
	return 0;
}
