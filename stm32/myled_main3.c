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
    GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_9;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(GPIOF, &GPIO_Initure);
}

void key_init(void)
{
    // 打开GPIOF的时钟
    __HAL_RCC_GPIOF_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_Initure;
    GPIO_Initure.Pin = GPIO_PIN_8;
    GPIO_Initure.Mode = GPIO_MODE_INPUT; // 模式：输入
    GPIO_Initure.Pull = GPIO_PULLUP;     // 上拉：没按时引脚默认高电平

    HAL_GPIO_Init(GPIOF, &GPIO_Initure);
}

int main(void)
{
    HAL_Init();                     // HAL库初始化
    Stm32_Clock_Init(336, 8, 2, 7); // 时钟配置到168MHZ
    delay_init(168);                // 延时初始化
    led_init();
    key_init();

    while (1)
    {
        HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_7);
        HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
        delay_ms(1000);
    }

    return 0;
}
