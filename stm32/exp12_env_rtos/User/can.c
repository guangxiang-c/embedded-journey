#include "can.h"

CAN_HandleTypeDef hcan1; // 句柄： HAL_CAN_Init 的户口本

void CAN1_Init(void)
{
	// 外设上电
	__HAL_RCC_GPIOA_CLK_ENABLE();
	//__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_CAN1_CLK_ENABLE();
	//__HAL_RCC_CAN2_CLK_ENABLE();

	GPIO_InitTypeDef gpio = {0};
	gpio.Pin = GPIO_PIN_11 | GPIO_PIN_12; // PA11=RX, PA12=TX, 复用AF9
	// gpio.Pin = GPIO_PIN_12 | GPIO_PIN_13; // PB12=RX, PB13=TX, 复用AF9
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_HIGH;
	// gpio.Alternate = GPIO_AF9_CAN2;
	gpio.Alternate = GPIO_AF9_CAN1;
	HAL_GPIO_Init(GPIOA, &gpio);

	// CAN参数：回环  42Mhz ÷ （BRP + 1） ÷ N = 1Mbps
	hcan1.Instance = CAN1;
	// hcan1.Instance = CAN2;
	hcan1.Init.Prescaler = 6;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = ENABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	HAL_CAN_Init(&hcan1);

	// 滤波器: 全收(掩码全0 = 来者不拒)
	CAN_FilterTypeDef filter = {0};
	filter.FilterBank = 0;
	filter.FilterMode = CAN_FILTERMODE_IDMASK;
	filter.FilterScale = CAN_FILTERSCALE_32BIT;
	filter.FilterIdHigh = 0;
	filter.FilterIdLow = 0;
	filter.FilterMaskIdHigh = 0;
	filter.FilterMaskIdLow = 0;
	filter.FilterFIFOAssignment = CAN_FilterFIFO0;
	filter.FilterActivation = ENABLE;
	filter.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan1, &filter);

	HAL_CAN_Start(&hcan1);
	// printf("BTR=%08x MSR=%08x\r\n", CAN1->BTR, CAN1->MSR);
}

uint8_t CAN1_Send(uint32_t id, uint8_t *data, uint8_t len)
{
	CAN_TxHeaderTypeDef tx = {0};
	tx.StdId = id;
	tx.IDE = CAN_ID_STD;
	tx.RTR = CAN_RTR_DATA;
	tx.DLC = len;
	uint32_t mailbox;
	return HAL_CAN_AddTxMessage(&hcan1, &tx, data, &mailbox) == HAL_OK; // 10ms 超时
}

uint8_t CAN1_Receive(uint32_t *id, uint8_t *data)
{
	if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) == 0) // 0 超时=纯查询
		return 0;
	CAN_RxHeaderTypeDef rx = {0};
	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx, data);
	*id = rx.StdId;
	return rx.DLC;
}