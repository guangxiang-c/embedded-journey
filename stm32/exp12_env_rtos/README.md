# exp12 - 环境监测节点 · RTOS 版（四任务双队列）

> 状态：? 完成（2026-10-29）。exp11 裸机版的 RTOS 重构——
> 四个 FreeRTOS 任务 + 两条队列，完成 传感器采集 → OLED 显示 → CAN 上报 → CAN 接收 全流程。
> 与 exp11 裸机版外在行为完全一致，内在架构完全不同。

## 一、成果

- OLED 实时温湿度（T/H 两行，1s 级刷新）+ 上位机每秒收到 0x131 帧（定点数协议）
- 反向链路：上位机发 0x200 → 板端 can_rx 任务串口打印
- 捂手测试：温度 +4℃、湿度 +9%，CAN 帧与屏幕同步爬升
- 与 exp11 的 A/B 价值：同一产品、裸机与 RTOS 双实现

## 二、任务架构

```javascript
                ┌── q_disp(深2) ──→ task_display ──→ OLED
                │                       （500ms 有界等待）
 task_sensor ───┤
  1s 采集        └── q_can(深2)  ──→ task_can_send ──→ CAN1_Send(0x131)
 （优先级3）                          （portMAX_DELAY 死等）

 task_can_rx（优先级1）：轮询接收 CAN → 串口打印
```

**数据流规则**：队列传值（拷贝），display 与 can 各拿独立副本——无共享变量、无数据竞争。

## 三、关键设计决策（面试可讲）

1. **一份数据两个消费者 → 两条队列**
队列每份数据只交付一人，sensor 向 q_disp、q_can 各发一份（拷贝两次）。
2. **等待策略匹配职责性质**

- display = 周期服务者 → 500ms 有界等待：sensor 挂掉也每半秒醒一次（自带看门狗属性，可显示 `--` 报警）
- can_send = 纯事件驱动者 → 死等：没数据时零 CPU 占用，数据到 0 延迟即发

3. **优先级梯度**：sensor(3) > display(2) = can_send(2) > can_rx(1)
数据源头最优先，打印最不急。

## 四、本次踩坑记录

| # | 坑 | 教训 |
| --- | --- | --- |
| 1 | RTOS 仓库 HAL 驱动组缺 i2c/can 源文件 → 9 个链接错误 | 新工程先检查外设源文件是否齐 |
| 2 | 新版 HAL 必须 `HAL_CAN_Start()` 才发送 | Init 配参数、Start 才上电（与 exp08 老 API 的差异） |
| 3 | 任务函数无 while(1) → return 即系统未定义行为 | 新任务第一行先搭 while(1) 骨架 |
| 4 | AHT20 忙时分支不赋值 → 首帧显示栈垃圾 | 输出参数函数先给默认值；局部变量零初始化 |

## 五、新旧 HAL CAN API 对照（迁移笔记）

| 老 API（exp08，HAL 1.7） | 新 API（本工程） |
| --- | --- |
| `CAN_InitTypeDef` + `BS1/BS2/SJW` | `TimeSeg1/TimeSeg2/SyncJumpWidth` |
| `HAL_CAN_Transmit(&hcan, timeout)` | `HAL_CAN_AddTxMessage(&hcan, &tx, data, &mailbox)` |
| `hcan.pTxMsg = &tx` 句柄挂载 | 栈上 `CAN_TxHeaderTypeDef` 直接传参 |
| `CAN_FilterConfTypeDef.FilterNumber` | `CAN_FilterTypeDef.FilterBank` |
| 无 Start | 滤波后必须 `HAL_CAN_Start()` |

## 六、与裸机版（exp11）的架构对比

|  | exp11 裸机 | exp12 RTOS |
| --- | --- | --- |
| 调度 | 主循环 + HAL_GetTick 软件门控 | 调度器抢占 + vTaskDelay |
| 任务节拍 | 共享循环时间（互相拖累） | 各自独立（改一个不影响别人） |
| 数据传递 | 全局变量 + 门控读取 | 双队列拷贝传递 |
| 代码耦合 | 三任务挤一个 while | 四任务各自一个函数 |

## 七、下一步

- [x] 环境监测节点裸机版（exp11）+ RTOS 版（exp12）
- [ ] CAN 命令下行：接收 ID 0x200 解析为控制指令（波特率/采样率远程配置）
- [ ] 传感器看门狗：连续 N 次无应答 → OLED 显示 SENSOR ERR