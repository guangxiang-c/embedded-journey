# embedded-journey

我的嵌入式自学之路：C语言 → STM32F407 → FreeRTOS → 项目实战

每章实验均经过"白纸设计 → 编码 → 实测 → commit"完整闭环，附踩坑记录。

## 实验索引

| 实验 | 内容 | 关键词 |
| --- | --- | --- |
| [exp02](stm32/exp02_myled/) | 按键消抖状态机 + 串口命令解析 | GPIO、UART、状态机 |
| [exp03](stm32/exp03_breath/) | PWM 呼吸灯（含 gamma 校正） | 定时器、PWM、独立重写 |
| [exp04](stm32/exp04_pwm_drv/) | 表驱动 PWM 驱动模块 | 函数指针、驱动设计 |
| [exp05](stm32/exp05_capture/) | 输入捕获测频 | 输入捕获、中断、回调 |
| [exp06](stm32/exp06_multikey/) | 多按键实例化 + 非阻塞呼吸 | static、实例化、节拍器 |
| [exp07](stm32/exp07_rtos1/) | FreeRTOS 移植 + 多任务 | 任务、vTaskDelay、arg 传参 |
| [exp08](stm32/exp08_can/README.md) | **CAN 双向通信 + 三天排障复盘** | 总线、仲裁、位时序、滤波器 ★ |
| [exp09](stm32/exp09_rtos2/) | 队列通信（生产者-消费者） | Queue、任务间通信 |
| [exp10](stm32/exp10_oled/) | I2C 一主多从：OLED + AHT20 | I2C、SSD1306、定宽刷新 |
| [exp11](stm32/exp11_env/) | **环境监测节点：温湿度 CAN 上报** | 综合实战：I2C+CAN+协议设计 ★ |

★ = 含完整 README 复盘，建议优先阅读

## 根目录 C 练习

`bubble_sort.c`、`global_test.c` 等 —— 跟翁恺 C 语言课程的配套练习（gcc 编译）

## 工具链

Keil MDK (AC6) + CMSIS-DAP / VS Code + gcc / PulseView + nanoDLA / 优利德 UT33B+

## 当前进度

I2C + CAN 双线打通，环境监测节点已跑通裸机版 → 下一步：FreeRTOS 任务化