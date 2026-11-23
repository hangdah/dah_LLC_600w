# dah_LLC_600w

基于 **TI TMS320F28335 DSP** 的 LLC 谐振变换器控制工程，面向微逆变器应用，使用 Code Composer Studio（CCS）开发。

## 主要功能

- 电压增量式 PI 控制，通过调节 PWM 周期实现调频控制。
- ePWM1 互补输出与死区控制。
- 四通道 ADC 软件触发采样与 IIR 滤波。
- 启动状态机及周期、死区渐变软启动。
- Timer0 以 10 μs 周期执行采样处理、PI 运算和 PWM 更新；Timer1 以 2.5 ms 周期执行状态机。

## 开发环境

- 目标芯片：TMS320F28335，CPU 时钟 150 MHz。
- 开发工具：CCS 12.8.0。
- 编译器：TI C2000 CGT 22.6.1.LTS，FPU32，COFF ABI。

## 工程结构

```text
main.c                  系统入口与初始化
APP/                    LLC 控制算法、采样及外设驱动
  APP_Libraries/        应用层头文件
DSP2833x_Librsries/     TI 外设支持库与链接脚本
targetConfigs/          CCS 目标配置
Debug/                  构建产物与自动生成文件
```

## 使用方法

1. 在 CCS 中导入本工程 `dah_LLC_600w`。
2. 检查工程的 TI 外设库头文件路径和编译器路径，按本机安装位置配置。
3. 确认所选构建配置中的链接脚本及 Flash/RAM 段配置，再执行 **Build Project**。

当前工程保留了部分 Boost 历史代码，其中的公共结构和 PWM 使能、关断函数仍被 LLC 使用。LLC 的软件过压、过流、欠压保护尚未接入当前运行状态，电流 PI 为占位实现，Burst 控制尚未接入主控制流程。

更多源码说明和修改约定见 [AGENTS.md](AGENTS.md)。

## 作者与链接

- 作者：hangdah
- 个人主页：[https://hangdah.github.io/](https://hangdah.github.io/)
- 项目地址：[https://github.com/hangdah/dah_LLC_600w](https://github.com/hangdah/dah_LLC_600w)
