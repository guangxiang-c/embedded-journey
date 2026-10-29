#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "freertos.h"
#include "queue.h"
#include "task.h"
#include "can.h"
#include "oled.h"

// 数据类型：一份环境数据
typedef struct
{
	int temp_x10; // 温度temperature
	int humi_x10; // 湿度humidity
} env_data_t;

QueueHandle_t q_disp, q_can;

// 温度采集
void task_sensor(void *arg)
{
	env_data_t env = {0}; // 零初始化
	while (1)
	{
		aht20_trigger_and_red(&env.temp_x10, &env.humi_x10); // 温湿度采集读取
		xQueueSend(q_disp, &env, 0);
		xQueueSend(q_can, &env, 0);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

// oled显示
void task_display(void *arg)
{
	env_data_t env;
	char buf[20];
	while (1)
	{
		if (xQueueReceive(q_disp, &env, pdMS_TO_TICKS(500)) == pdTRUE)
		{
			oled_show_str(0, 0, "welcome to AHT20");
			sprintf(buf, "T:%2d.%dC", env.temp_x10 / 10, env.temp_x10 % 10);
			oled_show_str(0, 1, buf);
			sprintf(buf, "H:%2d.%d%%", env.humi_x10 / 10, env.humi_x10 % 10);
			oled_show_str(0, 2, buf);
		}
		// 收不到（超时）就跳过本轮--屏幕保持旧值
	}
}

// can上报
void task_can_send(void *arg)
{
	env_data_t env;
	while (1)
	{
		if (xQueueReceive(q_can, &env, portMAX_DELAY) == pdTRUE) // 死等，有数据立刻发
		{
			uint8_t frame[8] = {env.temp_x10 >> 8, env.temp_x10 & 0xFF,
								env.humi_x10 >> 8, env.humi_x10 & 0xFF, 0, 0, 0, 0};
			CAN1_Send(0x131, frame, 8);
		}
	}
}

void task_can_receive(void *arg)
{
	uint32_t id;
	uint8_t rx[8];

	while (1)
	{
		uint8_t len = CAN1_Receive(&id, rx);
		if (len)
			printf("Rx ID:%x len:%d  data:%d %d %d %d\r\n", id, len, rx[0], rx[1], rx[2], rx[3]);
		vTaskDelay(pdMS_TO_TICKS(10)); // 轮询节奏 + 让低优先级任务有机会跑
	}
}

int main(void)
{
	HAL_Init();							/* 初始化HAL库 */
	sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz */
	delay_init(168);					/* 延时初始化 */
	usart_init(115200);					/* 串口初始化为115200 */

	CAN1_Init();
	oled_i2c_init();

	if (HAL_I2C_IsDeviceReady(&hi2c2, OLED_ADDR, 3, 100) == HAL_OK)
		printf("OLED ACK: 0x3C online\r\n");
	else
		printf("OLED NO ACK: check wiring/power/pullup\r\n");

	if (HAL_I2C_IsDeviceReady(&hi2c2, 0x38 << 1, 3, 100) == HAL_OK)
		printf("AHT20 ACK: 0x38 online\r\n");
	else
		printf("AHT20 NO ACK\r\n");

	oled_init();
	oled_clear();

	q_disp = xQueueCreate(2, sizeof(env_data_t));
	q_can = xQueueCreate(2, sizeof(env_data_t));

	xTaskCreate(task_sensor, "sensor", 256, NULL, 3, NULL);
	xTaskCreate(task_display, "display", 256, NULL, 2, NULL);
	xTaskCreate(task_can_send, "can_send", 256, NULL, 2, NULL);
	xTaskCreate(task_can_receive, "can_receive", 256, NULL, 1, NULL);
	vTaskStartScheduler();

	while (1)
		;
}
