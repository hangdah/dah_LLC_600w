# AGENTS.md

本文件适用于本工程目录及其子目录，供编码代理阅读、修改和验证工程时使用。默认使用中文与用户交流，新增源码注释使用中文。

## 工作原则

- 修改前先阅读相关源码、头文件和调用关系，理解当前实现后再动手。
- 只修改任务直接涉及的内容，优先选择最小范围修改；不要顺便重构、调参或修复无关问题。
- 保留已有变量名、函数名、接口名、文件名和目录结构。不要删除已有注释，除非注释明显错误或用户明确要求。
- 保持原文件的编码、换行、缩进和代码风格，禁止整文件重新格式化。读取中文乱码时先检查解码方式，例如使用 PowerShell 的 `Get-Content -Encoding UTF8`，不要直接重写文件。
- 保留用户已有修改；除非任务确有需要，不创建额外文件，不升级依赖、编译器或工具链，不擅改工程配置。
- 意图不明确且会影响控制行为或硬件配置时，先说明疑问。删除文件、覆盖大量代码或执行不可逆操作前询问用户。
- 完成后检查差异，简要说明涉及的文件、行为变化、验证结果和未验证事项。

## 源码修改范围

- 编写和修改代码仅限以下六组现有文件。其他源码可用于阅读和核对，确需修改时先取得用户的明确许可。

| 源文件 | 对应头文件 |
| --- | --- |
| `APP/control_algorithm.c` | `APP/APP_Libraries/control_algorithm.h` |
| `APP/control_config.c` | `APP/APP_Libraries/control_config.h` |
| `APP/control_pwm.c` | `APP/APP_Libraries/control_pwm.h` |
| `APP/control_sampling.c` | `APP/APP_Libraries/control_sampling.h` |
| `APP/control_state.c` | `APP/APP_Libraries/control_state.h` |
| `APP/llc_app.c` | `APP/APP_Libraries/llc_app.h` |

- 按职责把新功能写入对应模块。通用 PI 和 LLC 电压 PI 均放在 `control_algorithm.c/.h`，通用函数保留可复用接口，无需另建 `pi_controller.c/.h`。
- 新增或修改控制逻辑时，用中文注释说明模块职责、关键计算、限幅和时序。参数有量纲时注明单位，接口有边界条件时说明返回值和状态变化。

## 工程定位与资料依据

- 本工程是用于微逆变器应用的 LLC 谐振变换器控制固件，目标芯片为 TI TMS320F28335，C28x 内核，使用 FPU32。
- `.project` 中的实际工程名为 `dah_LLC_600w`，不要沿用 `CLAUDE.md` 示例中的 `dah_LLC_v3`。
- `CLAUDE.md` 可用于了解背景，但部分描述已与源码不同。执行路径以当前源码及实际初始化调用为准，构建配置以选定 CCS 配置及实际编译、链接命令为准。
- 当前控制链路为电压 PI 调频控制，快慢两级中断负责不同任务；不要据旧文档称其为已经实现的电压、电流双闭环。

## 目录与修改入口

| 路径 | 职责与注意事项 |
| --- | --- |
| `main.c` | 系统入口、Flash/RAM 准备、外设与控制参数初始化、定时器周期设置；主循环为空 |
| `APP/llc_app.c` | LLC 应用入口，持有快慢环共享的控制指令 |
| `APP/control_config.c`、`APP/APP_Libraries/control_config.h` | 控制默认值、参数和量纲 |
| `APP/control_algorithm.c` | 通用增量式 PI、抗饱和，以及 LLC 电压 PI 与周期换算 |
| `APP/control_sampling.c` | ADC 流水采样、滤波及输出电流零偏校准 |
| `APP/control_pwm.c` | PWM 计算、运行时寄存器更新及 Trip Zone 输出启停 |
| `APP/control_state.c` | 主状态机、软启动子状态及预留的 Burst 实现 |
| `APP/timer0.c`、`APP/timer1.c` | 快速控制中断、慢速状态机中断及各自初始化 |
| `APP/adc.c`、`APP/epwm.c` | ADC 与 ePWM 寄存器初始化 |
| `APP/led.c`、`key.c`、`relay.c`、`exit.c`、`mppt.c` | LED、按键、继电器、外部中断、MPPT；是否实际启用须检查调用路径 |
| `APP/APP_Libraries/` | 应用层头文件；头文件不与 `.c` 文件同目录 |
| `DSP2833x_Librsries/` | TI 外设支持源码、汇编、链接脚本和 `IQmath.lib`；保留现有目录拼写 |
| `.project`、`.cproject`、`.ccsproject`、`targetConfigs/` | CCS 工程、工具链与目标配置 |
| `Debug/` | CCS 自动生成的 makefile、依赖及构建产物，不作为源码修改入口 |

控制入口集中在 `llc_app.h`。模块内状态优先使用文件作用域的 `static`，快慢环共享的控制指令由 `llc_app.c` 持有。保留 `g_Soft_State_M_for_Graph` 供 CCS Graph 观察。

## 当前实际控制路径

### 启动与中断

- `main()` 通过 `_FLASHOK` 条件编译执行 RAM 函数复制及 `InitFlash()`，并初始化系统时钟、PIE、LED、LLC 参数、Timer0/1、ePWM1 和 ADC。
- 外设配置涉及 `TBCLKSYNC` 的关闭和恢复，启动过程中调用 `PWM_Disable()`。修改初始化顺序时须追踪定时器启动、中断使能和 PWM 输出状态。
- `Timer0_Init()` 与 `Timer1_Init()` 内部也调用 `EINT`、`ERTM`，不能仅依据 `main()` 末尾的中断使能语句推断初始化全过程均无中断。
- Timer0 使用 `LLC_FAST_PERIOD_US`，当前周期为 **10 us / 100 kHz**。
- `Timer0_IRQn()` 翻转 LED1，调用 `LLC_App_FastTick()`，然后应答 PIE Group1。快环依次读取上一帧并触发下一帧、执行电压 PI、更新 PWM。
- Timer1 使用 `LLC_SLOW_PERIOD_US`，当前周期为 **2.5 ms / 400 Hz**。`Timer1_IRQn()` 翻转 LED2，经 `LLC_App_SlowTick()` 调用 `LLC_State_M()`。
- 首次读到有效 ADC 结果后才执行电压 PI。之后若某次转换尚未完成，快环沿用最近一次有效采样。

### 主状态机与软启动

- `LLC_State_M()` 使用 `LLC_Flag.StateM` 和 `control_state.c` 内部的 `STATE_M`：`Init → Wait → Rise → Run`，另有 `Err` 分支。
- `Init` 中进行参数初始化和 `IoutOffsetCal()`；偏移校准累计 256 次后完成。
- `Wait` 中无错误时累计 600 个 Timer1 周期，约 1.5 s 后进入 `Rise`。
- `Rise` 内部才使用 `Soft_State_M`：`SSInit → SSWait → SSRun`。不要将此枚举误写为 LLC 主状态机。
- `SSWait` 条件为计数 `> 200`，按当前 2.5 ms 调用周期约为 502.5 ms。
- `SSRun` 逐步增大允许的最大周期并减小死区，达到目标后转入主状态 `Run`。
- `LLC_StateMRun()` 当前为空；`LLC_StateMErr()` 调用 `PWM_Disable()`。
- `LLC_BurstCtl()` 保留在状态模块，当前慢环未调用。

## 控制参数与单位

- `LLC_CONTROL_RATE_HZ` 由 Timer0 的周期配置推导，并用于计算 PI 的 `LLC_V_KI_TS`。修改控制周期时一并核对滤波及相关计时。
- `LLC_RESONANT_FREQ` 与 `LLC_RESONANT_CYCLE_US` 对应 400 kHz / 2.5 us，分别用于 PWM 初始化及 PI 中心点。修改谐振点时同时检查两个配置值和周期限幅。
- 当前电压 PI 初始化为 `Kp = 150.0f`、`Ki = 150000.0f`，误差限幅为 ±20，PI 输出限幅为 `[-0.83, 1.5]` us。
- 周期控制关系为 `Ctrl_CycleTime = LLC_RESONANT_CYCLE_US - u_sat`，再受动态周期上下限约束。当前常规范围为 1.0–3.33 us，软启动会改变最大周期。修改 PI 符号前先核对工作区间与实际功率级响应。
- `Ctrl_CycleTime` 和 `PWM_CycleTime` 使用 us，`Ctrl_DeadTime` 使用 ns。PWM 初始 `TBPRD` 截断取整，运行时更新采用四舍五入。
- ePWM1 使用上下计数、时基不分频，按当前 150 MHz 时钟，更新公式为 `TBPRD ≈ 75 × Ctrl_CycleTime(us)`；`CMPA` 约为 `TBPRD / 2`。
- 死区计数换算为 `dead_time(ns) × 0.15` 并取整。修改 CPU 时钟、分频或计数模式时必须重新核对这些换算和限幅。
- PI 模块管理运算状态，PWM 模块负责周期和死区换算以及运行时寄存器写入。

## ADC 与保护边界

- 当前 `ADC_Init()` 配置 SEQ1 四通道、顺序采样、单次序列，关闭 ePWM SOCA 和外部触发，关闭 SEQ1 的 CPU 中断使能。
- 当前由 Timer0 中的软件流程读取上一帧完成结果、清除完成标志，然后复位并启动下一次 SEQ1。
- 通道映射：`ADCRESULT0 / ADCINA0 → Vout`，`ADCRESULT1 / ADCINA1 → IL`，`ADCRESULT2 / ADCINB0 → Vin`，`ADCRESULT3 / ADCINB1 → Iout`；结果读取右移 4 位。
- `ADC_Transform = 3.0f / 4095.0f`，`IIR_ALPHA = 0.001f`。当前 LLC 的 `Vout` 直接取滤波后的 ADC 电压，另外三路乘 `44.4444f`；这只是源码现状，不代表硬件标定已确认。修改采样量或参考值前先核对量纲和模拟前端比例。
- `PWM_Disable()` 通过 ePWM1 `TZFRC.OST` 强制关断；`PWM_Enable()` 清除 `TZCLR.OST`。ePWM1 配置了 TZ1 一次性触发、A/B 输出强制低电平；硬件引脚和外部保护链是否有效须另行确认。
- LLC 运行状态暂未接入软件过压、过流、欠压和短路保护。
- 新增 LLC 保护时须明确采样来源、阈值单位、调用周期、故障标志、关断和恢复路径；延时按实际调用频率计算，不能直接照搬旧注释。

## 实时性与硬件修改规则

- 优先保证嵌入式 C 的可读性、实时性与可维护性，沿用 TI 类型、寄存器访问方式和当前工程风格。
- 不在快速 ISR 中引入阻塞等待、延时、动态内存、文件操作或大量输出；新增计算要考虑 10 us 的控制周期预算。
- 修改 Timer0/1 共用变量前，检查访问时序、可见性及一致性；不要仅靠添加 `volatile` 就声称解决了共享数据问题。
- 保留已有 `#pragma CODE_SECTION(..., ".TI.ramfunc")` 及 RAM 段约定。函数是否实际在 RAM 执行还须看链接结果，不能仅凭注释判断。
- 不随意修改 PWM 极性、死区、Trip Zone、GPIO、ADC 触发与时钟、中断向量和应答、定时器或 DMA 配置。任务确需调整时，先核对上下文和用途，并说明时序与控制影响。
- 修改控制算法后明确列出新增或修改的变量、参数、函数及作用，避免将实验调参混入其他修复。

## 构建与验证

- 工程记录的环境为 CCS 12.8.0、TI C2000 CGT 22.6.1.LTS；当前 Debug 命令使用 `-v28 -ml -mt --float_support=fpu32 --abi=coffabi`，栈设置为 `--stack_size=0x300`。
- 优先在 CCS 中导入工程，选择所需配置后执行 Build Project。若使用 CCS 命令行构建，应先确认本机安装位置、工作空间和工程已导入，工程参数使用 `dah_LLC_600w`。
- `Debug/makefile` 是 CCS 自动生成文件，现有环境也可通过 CCS 配套的 make 工具构建；不要手工维护生成的 makefile 或将其作为配置修改的替代方式。
- 工程依赖外部 TI 头文件目录：`D:\AAAstudy\DSPF28335_Program\DSP2833x_Libraries\DSP2833x_common\include` 和 `DSP2833x_headers\include`，具体完整路径见 `.cproject`。缺失时如实报告，不擅自复制其他版本或改用主机 GCC 证明固件可用。
- 工程元数据仍记录 `28335_RAM_lnk.cmd`，但当前目录未包含该脚本；现有 `Debug/makefile` 实际输入为 `DSP2833x_Librsries/F28335.cmd` 和 `DSP2833x_Headers_nonBIOS.cmd`，运行库为 `rts2800_fpu32.lib`。不要沿用旧文档“Debug 必为 RAM 链接、Release 必为 Flash 链接”的假定，也不要将 Release 自动视为已验证的生产配置。
- `_FLASHOK` 当前在 `control_config.h` 定义；`main.c` 中的复制符号必须与链接脚本的 LOAD/RUN/SIZE 配置匹配。涉及 RAM 函数或链接布局时，检查 map 文件中的段位置、复制范围和 RAM 容量。
- 当前工程未提供独立自动化测试入口。代码修改后进行最小必要检查，并在工具链可用时编译相关配置；纯文档修改以交叉核对源码和检查文件差异为主。
- 编译通过不代表 ADC 标定、控制稳定性、软启动、故障保护或 ISR 耗时已经上板验证。无法编译或上板时明确说明，不声称功能正常；验证失败时报告原因，不擅改无关内容。
- 有 Git 元数据时使用 `git status`、`git diff` 检查改动；当前目录可能是无 `.git` 的工程副本，此时使用修改前后内容或文件哈希比对，不声称已完成 Git 差异检查。
- 未经用户明确要求，不执行 `git commit`、`git push`、`git reset`、`git rebase`，不清理或丢弃已有工作成果。
