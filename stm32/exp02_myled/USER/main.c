#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "string.h"

int main(void)
{
    HAL_Init();                     // HAL库初始化
    Stm32_Clock_Init(336, 8, 2, 7); // 时钟配置到168MHZ
    delay_init(168);                // 延时初始化
    uart_init(115200);              // 初始化USART
    led_init();
    key_init();
    uint32_t last_blink = 0; // 上次翻灯时刻
    u8 len;
    u16 times = 0;

    while (1)
    {
        uint32_t now = HAL_GetTick(); // 现在几点（毫秒）

        // 按键控制-面包板红灯
        if (key_scan())
        {
            HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_7);
        }

        // 板子红灯-心跳
        if (now - last_blink >= 500)
        {
            last_blink = now;
            HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
        }

        // 串口通信
        if (USART_RX_STA & 0x8000) // 收到完整一帧（以回车结束）
        {
            len = USART_RX_STA & 0x3fff; // 得到此次接收到的数据长度

            char content[20];
            memcpy(content, (char *)USART_RX_BUF, len); // 拷贝到本地
            content[len] = '\0';                        // 加字符串结束符

            // 去掉尾部的回车符
            if (len > 0 && (content[len - 1] == '\r' || content[len - 1] == '\n'))
            {
                content[len - 1] = '\0';
            }

            printf("\r\nCommand:%s\r\n", content);

            // 控制板子绿灯
            if (strcmp(content, "on") == 0)
            {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET); // 绿灯亮
                printf("LED ON\r\n");
            }
            if (strcmp(content, "off") == 0)
            {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET); // 绿灯灭
                printf("LED OFF\r\n");
            }

            USART_RX_STA = 0;
        }
        else
        {
            times++;
            if (times % 200 == 0)
                printf("Please input data, end with Enter\r\n");
            delay_ms(10);
        }
    }
		return 0;
}
