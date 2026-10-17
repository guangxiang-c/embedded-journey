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
    // GPIO_Initure.Pin = GPIO_PIN_9 | GPIO_PIN_10; // PF9-LED0-红灯  PF10-LED1-绿灯
    GPIO_Initure.Pin = GPIO_PIN_9; // PF9-LED0-红灯  PF10-LED1-绿灯
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
    int key_state = 0;
    int key_confirm = 0;
    int key_state_before = 0;
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET); // 初始状态红灯灭

    while (1)
    {
        // 按键控制红灯亮
        // 读PF8的电平：按下=0（接地），松开=1（上拉）
        key_state = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8) == GPIO_PIN_RESET); // 1-按键按下;0-按键松开
        delay_ms(20);
        key_confirm = (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8) == GPIO_PIN_RESET);
        if (key_state == 1 && key_confirm == 1 && key_state_before == 0)
        {
            HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
        }
        key_state_before = key_state;
    }

    return 0;
}
