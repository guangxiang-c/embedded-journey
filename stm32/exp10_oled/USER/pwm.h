#ifndef _PWM_H
#define _PWM_H
#include "sys.h"

struct pwm_table_item
{
	uint32_t pin;              // 引脚
	GPIO_TypeDef *port;		     // 引脚端口号
	TIM_TypeDef *timer;	       // 定时器
	uint32_t channel;	         // 定时器通道号
	uint32_t reuse_number;     // 复用编号
	void(*clk_on)(void);       // 定时器使能函数
	uint32_t gpio_clk_bit;     // 
	u16 arr;                   // 
	u16 psc;                   // 
};

void pwm_setup_all();
void pwm_set(int id, uint32_t compare);

#endif