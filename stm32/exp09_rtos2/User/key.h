#ifndef _KEY_H
#define _KEY_H
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"

#define KEY_SAMPLE_MS 10       // 采样周期
#define KEY_DEBOUNCE_CNT 3     // 消抖需要的连续一致采样次数
#define KEY_LONG_PRESS_MS 1000 // 长按门槛

typedef enum
{
    KEY_NONE = 0, // 无事件
    KEY_SHORT,    // 短按
    KEY_LONG      // 长按
} key_event_t;

struct key_instance {
	// 硬件身份证
	GPIO_TypeDef *port;
	uint16_t pin;
	void(*clk_on)(void);
	// 状态记忆
	uint32_t last_sample; // 上次采样时刻
	int stable;			 // 已确认的稳定状态
	int raw;				 // 上次原始读数
	int same_cnt;		 // 连续相同次数
	uint32_t press_time;	 // 按下时刻时间戳
	int long_reported;	 // 本次按住是否已经上报过长按
};

void key_setup_all(void);
key_event_t key_scan(int id);


#endif