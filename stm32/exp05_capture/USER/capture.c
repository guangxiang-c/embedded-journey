#include "capture.h"

TIM_HandleTypeDef TIM4_Handler; // 定时器 4 句柄

// 定时器 5 通道 1 输入捕获配置
// arr：自动重装值(TIM4 是 32 位的!!)
// psc：时钟预分频数
void TIM4_CH1_Cap_Init(u32 arr, u16 psc)
{
	TIM_IC_InitTypeDef TIM4_CH1Config;
	TIM4_Handler.Instance = TIM4;											 // 通用定时器 5
	TIM4_Handler.Init.Prescaler = psc;										 // 分频系数
	TIM4_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;						 // 向上计数器
	TIM4_Handler.Init.Period = arr;											 // 自动装载值
	TIM4_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;				 // 时钟分频银子
	HAL_TIM_IC_Init(&TIM4_Handler);											 // 初始化输入捕获时基参数
	TIM4_CH1Config.ICPolarity = TIM_ICPOLARITY_RISING;						 // 上升沿捕获
	TIM4_CH1Config.ICSelection = TIM_ICSELECTION_DIRECTTI;					 // 映射到 TI1 上
	TIM4_CH1Config.ICPrescaler = TIM_ICPSC_DIV1;							 // 配置输入分频，不分频
	TIM4_CH1Config.ICFilter = 0;											 // 配置输入滤波器，不滤波
	HAL_TIM_IC_ConfigChannel(&TIM4_Handler, &TIM4_CH1Config, TIM_CHANNEL_1); // 配置 TIM4 通道 1
	HAL_TIM_IC_Start_IT(&TIM4_Handler, TIM_CHANNEL_1);						 // 开启 TIM4 的捕获通道 1，并且开启捕获中断
	__HAL_TIM_ENABLE_IT(&TIM4_Handler, TIM_IT_UPDATE);						 // 使能更新中断
}

// 定时器 4 底层驱动，时钟使能，引脚配置
// 此函数会被 HAL_TIM_IC_Init()调用
// htim:定时器 4 句柄
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM4_CLK_ENABLE();			// 使能 TIM4 时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();			// 开启 GPIOB 时钟
	GPIO_Initure.Pin = GPIO_PIN_6;			// PB6
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;	// 复用推挽输出
	GPIO_Initure.Pull = GPIO_PULLDOWN;		// 下拉
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;	// 高速
	GPIO_Initure.Alternate = GPIO_AF2_TIM4; // PB6 复用为 TIM4 通道 1
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);
	HAL_NVIC_SetPriority(TIM4_IRQn, 2, 0); // 设置中断优先级，抢占优先级 2，子优先级 0
	HAL_NVIC_EnableIRQ(TIM4_IRQn);		   // 开启 TIM4 中断通道
}

// 捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于 32 位定时器来说,1us 计数器加 1,溢出时间:4294 秒)
u8 TIM4CH1_CAPTURE_STA = 0; // 输入捕获状态
u32 TIM4CH1_CAPTURE_VAL;	// 输入捕获值(TIM4 是 32 位)
// 定时器 4 中断服务函数
void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM4_Handler); // 定时器共用处理函数
}

// 定时器更新中断（计数溢出）中断处理回调函数，
// 该函数在 HAL_TIM_IRQHandler 中会被调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// 更新中断（溢出）发生时执行
{
	if ((TIM4CH1_CAPTURE_STA & 0X80) == 0) // 还未成功捕获
	{
		if (TIM4CH1_CAPTURE_STA & 0X40) // 已经捕获到高电平了
		{
			if ((TIM4CH1_CAPTURE_STA & 0X3F) == 0X3F) // 高电平太长了
			{
				TIM4CH1_CAPTURE_STA |= 0X80; // 标记成功捕获了一次
				TIM4CH1_CAPTURE_VAL = 0XFFFFFFFF;
			}
			else
				TIM4CH1_CAPTURE_STA++;
		}
	}
}

static u32 last = 0;

// 定时器输入捕获中断处理回调函数，该函数在 HAL_TIM_IRQHandler 中会被调用
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) // 捕获中断发生时执行
{
	u32 now = HAL_TIM_ReadCapturedValue(&TIM4_Handler, TIM_CHANNEL_1); // 获取当前的捕获值
	
	if ((TIM4CH1_CAPTURE_STA & 0X40) == 0) // 还没有基准值
	{
		last = now;
		TIM4CH1_CAPTURE_STA |= 0X40;
	}
	else 
	{
		TIM4CH1_CAPTURE_VAL = now - last;
		last = now;
		TIM4CH1_CAPTURE_STA |= 0X80;	  // 周期
	}
		
}