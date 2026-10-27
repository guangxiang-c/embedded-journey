#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "can.h"

int main()
{

  HAL_Init();                     // 初始化HAL库
  Stm32_Clock_Init(336, 8, 2, 7); // 初始化时钟168Mhz
  delay_init(168);                // 初始化延时函数
  uart_init(115200);              // 初始化 USART
	
	CAN1_Init();
	printf("boot 500k\r\n");
	uint8_t tx[8] = {1,2,3,4,5,6,7,8};
	int cnt = 0;
	while (1)
	{
			tx[0] = cnt;
			uint8_t ok = CAN1_Send(0x131, tx, 8);
			printf("TX ok=%d cnt=%d\r\n", ok, cnt);
			delay_ms(1000);
			cnt++;

			uint32_t id;  uint8_t rx[8];
			uint8_t len = CAN1_Receive(&id, rx);
			if (len)
					printf("Rx ID:%x len:%d  data:%d %d %d %d\r\n", id, len, rx[0], rx[1], rx[2], rx[3]);
	}

  return 0;
}