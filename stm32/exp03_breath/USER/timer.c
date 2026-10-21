#include "timer.h"

TIM_HandleTypeDef htim14; // 定时器句柄
TIM_HandleTypeDef htim11; // 定时器句柄
TIM_OC_InitTypeDef sConfigOC;

void breath_pwm_init(u16 arr, u16 psc)
{
	// 外设上电
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_TIM14_CLK_ENABLE();
	
	__HAL_RCC_TIM11_CLK_ENABLE();

	// PF9引脚换身份
	GPIO_InitTypeDef GPIO_Initure;
	GPIO_Initure.Pin = GPIO_PIN_9;
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	GPIO_Initure.Alternate = GPIO_AF9_TIM14;
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);
	
	// PF7引脚换身份
	GPIO_InitTypeDef GPIO_Initure_PF7;
	GPIO_Initure_PF7.Pin = GPIO_PIN_7;
	GPIO_Initure_PF7.Mode = GPIO_MODE_AF_PP;
	GPIO_Initure_PF7.Pull = GPIO_PULLUP;
	GPIO_Initure_PF7.Speed = GPIO_SPEED_HIGH;
	GPIO_Initure_PF7.Alternate = GPIO_AF3_TIM11;
	HAL_GPIO_Init(GPIOF, &GPIO_Initure_PF7);

	// PWM初始化-定节奏
	htim14.Instance = TIM14;
	htim14.Init.Prescaler = psc;
	htim14.Init.Period = arr;
	htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
	
	htim11.Instance = TIM11;
	htim11.Init.Prescaler = psc;
	htim11.Init.Period = arr;
	htim11.Init.CounterMode = TIM_COUNTERMODE_UP;

	
	HAL_TIM_PWM_Init(&htim14); // PWM初始化
	HAL_TIM_PWM_Init(&htim11); // PWM初始化

	// 选模式+按开关
	sConfigOC.OCMode = TIM_OCMODE_PWM1;		   // CNT<CCR 输出有效电平
	sConfigOC.Pulse = arr / 2;				   // 初始占空比50%
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW; // 低有效-适配低电平点亮的板载LED
	HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1); // 通道接引脚 + 开始计数
	
	HAL_TIM_PWM_ConfigChannel(&htim11, &sConfigOC, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1); // 通道接引脚 + 开始计数
}