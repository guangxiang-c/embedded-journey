/* ============ oled.c ==========*/
#include "oled.h"

I2C_HandleTypeDef hi2c2;

#define OLED_ADDR  (0x3C << 1)  // HAL要8位地址（左移过）
#define CTRL_CMD   0x00         // 控制字节：0x00 = 后面是命令
#define CTRL_DATA  0x40         // 控制字节：0x40 = 后面是显示数据

// I2C2: PB10=SCL, PB11=SDA, AF4
void oled_i2c_init(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C2_CLK_ENABLE();
	
	GPIO_InitTypeDef gpio ={0};
	gpio.Pin = GPIO_PIN_10 | GPIO_PIN_11;
	gpio.Mode = GPIO_MODE_AF_OD;   // I2C用开漏！（上拉电阻负责拉高）
	gpio.Pull = GPIO_PULLUP;       // 内部上拉先撑着，模块自带上拉更佳
	gpio.Speed = GPIO_SPEED_HIGH;
	gpio.Alternate = GPIO_AF4_I2C2;
	HAL_GPIO_Init(GPIOB, &gpio);
	
	hi2c2.Instance = I2C2;
	hi2c2.Init.ClockSpeed = 400000;  // 400kHz 快速模式
	hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c2.Init.OwnAddress1 = 0;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	HAL_I2C_Init(&hi2c2);
}

// 发1条命令
void oled_cmd(uint8_t cmd)
{
	uint8_t buf[2] = {CTRL_CMD, cmd};
	HAL_I2C_Master_Transmit(&hi2c2, OLED_ADDR, buf, 2, 100);
}

// 上电初始化序列（SSD1306 通用版）
void oled_init(void)
{
	static const uint8_t seq[] = {
		0xAE,        // 关显示
		0xD5, 0x80,  // 显示时钟
		0xA8, 0x3F,  // 复用率64
		0xD3, 0x00,  // 显示偏移
		0x40,        // 起始行
		0x8D, 0x14,  // 电荷泵使能（关键！不开启屏不亮）
		0x20, 0x02,  // 内存模式 = 页模式
		0xA1,        // 段重映射
		0xC8,        // COM 扫描方向
		0xDA, 0x12,  // COM 引脚配置
		0x81, 0xCF,  // 对比度
		0xD9, 0xF1,  // 预充电
		0xDB, 0x40,  // VCOMH
		0xA4,        // 显示跟随 RAM
		0xA6,        // 正显（不反色）
		0xAF         // 开显示
	};
	for (uint16_t i = 0; i < sizeof(seq); i++)
		oled_cmd(seq[i]);
}

// 清屏：8页 × 128字节全写0
void oled_clear(void)
{
	for(uint8_t page = 0; page < 8; page++)
	{
		oled_cmd(0xB0 + page);  // 选页
		oled_cmd(0x00);         // 列地址低
		oled_cmd(0x10);         // 列地址高
		uint8_t buf[129];
		buf[0] = CTRL_DATA;
		for(uint8_t i = 1; i < 128; i++) buf[i] = 0xFF;
	  HAL_I2C_Master_Transmit(&hi2c2, OLED_ADDR, buf, 129, 100);
	}
}