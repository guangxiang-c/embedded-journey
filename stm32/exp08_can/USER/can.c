#include "can.h"

CAN_HandleTypeDef hcan1; // 句柄： HAL_CAN_Init 的户口本

void CAN1_Init(void)
{
	// 外设上电
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_CAN1_CLK_ENABLE();

	// PB8=RX, PB9=TX, 复用AF9
	GPIO_InitTypeDef gpio = {0};
	gpio.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_HIGH;
	gpio.Alternate = GPIO_AF9_CAN1;
	HAL_GPIO_Init(GPIOB, &gpio);
	
	// CAN参数：回环  42Mhz ÷ （BRP + 1） ÷ N = 1Mbps
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 5;
	hcan1.Init.Mode = CAN_MODE_LOOPBACK;
	hcan1.Init.SJW = CAN_SJW_1TQ;
	hcan1.Init.BS1 = CAN_BS1_4TQ;
	hcan1.Init.BS2 = CAN_BS2_2TQ;
	hcan1.Init.TTCM = DISABLE;
	hcan1.Init.ABOM = DISABLE;
	hcan1.Init.AWUM = DISABLE;
	hcan1.Init.RFLM = DISABLE;
	hcan1.Init.TXFP = DISABLE;
	hcan1.Init.NART = DISABLE;
	HAL_CAN_Init(&hcan1);
	
	// 滤波器: 全收(掩码全0 = 来者不拒)
	CAN_FilterConfTypeDef filter = {0};
	filter.FilterNumber = 0;
	filter.FilterMode = CAN_FILTERMODE_IDMASK;
	filter.FilterScale = CAN_FILTERSCALE_32BIT;
	filter.FilterIdHigh = 0;
	filter.FilterIdLow = 0;
	filter.FilterMaskIdHigh = 0;
	filter.FilterMaskIdLow = 0;
	filter.FilterFIFOAssignment = CAN_FilterFIFO0;
	filter.FilterActivation = ENABLE;
	filter.BankNumber = 14;
	HAL_CAN_ConfigFilter(&hcan1, &filter);
}
	
uint8_t CAN1_Send(uint32_t id, uint8_t *data, uint8_t len){
		CanTxMsgTypeDef tx = {0};
    tx.StdId = id;
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = len;
    for (uint8_t i = 0; i < len; i++) tx.Data[i] = data[i];
    hcan1.pTxMsg = &tx;
    return HAL_CAN_Transmit(&hcan1, 10) == HAL_OK;   // 10ms 超时
}

uint8_t CAN1_Receive(uint32_t *id, uint8_t *data)
{
    CanRxMsgTypeDef rx = {0};
    hcan1.pRxMsg = &rx;
    if (HAL_CAN_Receive(&hcan1, CAN_FIFO0, 0) != HAL_OK)  // 0 超时=纯查询
        return 0;
    *id = rx.StdId;
    for (uint8_t i = 0; i < rx.DLC; i++) data[i] = rx.Data[i];
    return rx.DLC;
}