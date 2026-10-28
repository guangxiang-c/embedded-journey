#ifndef _OLED_H
#define _OLED_H
#include "sys.h"
#define OLED_ADDR  (0x3C << 1)

void oled_i2c_init(void);
void oled_cmd(uint8_t cmd);
void oled_init(void);
void oled_clear(void);
void oled_show_char(uint8_t x, uint8_t y, char ch);
void oled_show_str(uint8_t x, uint8_t y, const char *s);

extern I2C_HandleTypeDef hi2c2;

#endif