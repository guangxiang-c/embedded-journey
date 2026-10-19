#ifndef __KEY_H
#define __KEY_H

#include "sys.h"
#include "delay.h"

#define KEY_SAMPLE_MS 10       // 采样周期
#define KEY_DEBOUNCE_CNT 3     // 消抖需要的连续一致采样次数
#define KEY_LONG_PRESS_MS 1000 // 长按门槛

typedef enum
{
    KEY_NONE = 0, // 无事件
    KEY_SHORT,    // 短按
    KEY_LONG      // 长按
} key_event_t;

void key_init(void);
key_event_t key_scan(void);

#endif