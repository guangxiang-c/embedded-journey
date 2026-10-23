#include "pwm.h"

static void tim14_clk_on(void) { __HAL_RCC_TIM14_CLK_ENABLE(); };
static void tim11_clk_on(void) { __HAL_RCC_TIM11_CLK_ENABLE(); };
static void tim3_clk_on(void) { __HAL_RCC_TIM3_CLK_ENABLE(); };
TIM_HandleTypeDef htim; // 定时器句柄
TIM_OC_InitTypeDef sConfigOC;

static struct pwm_table_item pwm_table[] = {
    {GPIO_PIN_9, GPIOF, TIM14, TIM_CHANNEL_1, GPIO_AF9_TIM14, tim14_clk_on, RCC_AHB1ENR_GPIOFEN, 499, 83},
    {GPIO_PIN_7, GPIOF, TIM11, TIM_CHANNEL_1, GPIO_AF3_TIM11, tim11_clk_on, RCC_AHB1ENR_GPIOFEN, 499, 83},
    {GPIO_PIN_6, GPIOA, TIM3, TIM_CHANNEL_1, GPIO_AF2_TIM3, tim3_clk_on, RCC_AHB1ENR_GPIOAEN, 499, 83}};

void pwm_setup_all()
{
    int len = sizeof(pwm_table) / sizeof(pwm_table[0]);
    for (int i = 0; i < len; i++)
    {
        // 外设上电
        pwm_table[i].clk_on();
        RCC->AHB1ENR |= pwm_table[i].gpio_clk_bit;

        // 引脚换身份
        GPIO_InitTypeDef GPIO_Initure;
        GPIO_Initure.Pin = pwm_table[i].pin;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;
        GPIO_Initure.Pull = GPIO_PULLUP;
        GPIO_Initure.Speed = GPIO_SPEED_HIGH;
        GPIO_Initure.Alternate = pwm_table[i].reuse_number;
        HAL_GPIO_Init(pwm_table[i].port, &GPIO_Initure);

        // PWM初始化-定节奏
        htim.Instance = pwm_table[i].timer;
        htim.Init.Prescaler = pwm_table[i].psc;
        htim.Init.Period = pwm_table[i].arr;
        htim.Init.CounterMode = TIM_COUNTERMODE_UP;

        HAL_TIM_PWM_Init(&htim); // PWM初始化

        // 选模式+按开关
        sConfigOC.OCMode = TIM_OCMODE_PWM1;        // CNT<CCR 输出有效电平
        sConfigOC.Pulse = pwm_table[i].arr / 2;    // 初始占空比50%
        sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW; // 低有效-适配低电平点亮的板载LED
        HAL_TIM_PWM_ConfigChannel(&htim, &sConfigOC, pwm_table[i].channel);
        HAL_TIM_PWM_Start(&htim, pwm_table[i].channel); // 通道接引脚 + 开始计数
    }
}

void pwm_set(int id, uint32_t compare)
{
    TIM_TypeDef *tim = pwm_table[id].timer;
    switch (pwm_table[id].channel)
    {
    case TIM_CHANNEL_1:
        tim->CCR1 = compare;
        break;
    }
}