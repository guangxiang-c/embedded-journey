#ifndef _CAN_H
#define _CAN_H
#include "./SYSTEM/sys/sys.h"

void CAN1_Init(void);
uint8_t CAN1_Send(uint32_t id, uint8_t *data, uint8_t len);
uint8_t CAN1_Receive(uint32_t *id, uint8_t *data);

#endif
