# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 提供本代码仓库的工作指导。

## 项目概述

本项目是针对 **TI TMS320F28335 DSP** 的嵌入式 C 项目，用于控制 **LLC 谐振变换器**（微逆变器应用）。项目使用 Code Composer Studio (CCS) 作为开发环境。

## 构建系统

本项目使用 **Code Composer Studio (CCS)** 配合 TI C2000 编译器工具链：
- 编译器：TI C2000 CGT 22.6.1.LTS
- 目标芯片：TMS320F28335（C28x 内核，带 FPU32）
- 输出格式：COFF
- 链接器命令文件：`28335_RAM_lnk.cmd`（Debug）、`F28335.cmd`（Release）

### 构建方法

构建必须在 CCS IDE 中完成或使用 CCS 命令行工具：
```bash
# 导入项目到 CCS 并从 IDE 构建
# 或使用 CCS 命令行（如已配置）
"${CCS_INSTALL}/eclipse/eclipsec" -noSplash -data "${WORKSPACE}" -application com.ti.ccstudio.apps.projectBuild -ccs.projects dah_LLC_v3 -ccs.configuration Debug
```

### 关键构建配置

来自 `.cproject`：
- **Debug**：使用 `28335_RAM_lnk.cmd`，针对调试优化
- **Release**：使用 `F28335.cmd`，针对生产优化
- 堆栈大小：0x300 字节
- 运行时库：`rts2800_fpu32.lib`

### 编译器标志

来自 `.cproject` 的关键选项：
- `--large_memory_model`（已弃用，默认设置）
- `--unified_memory` (-mt)
- `--silicon_version=28`
- `--float_support=fpu32`
- `--abi=coffabi`

## 项目结构

```
dah_LLC_v3/
├── main.c                    # 入口点，系统初始化
├── APP/                      # 应用层
│   ├── adc.c/h              # ADC 驱动，用于电压/电流采样
│   ├── epwm.c/h             # ePWM 驱动，用于 LLC 开关控制
│   ├── timer0.c/h           # CPU 定时器 0（快速控制环，~20us）
│   ├── timer1.c/h           # CPU 定时器 1（慢速控制环，2.5ms = 400Hz）
│   ├── function.c/h         # Boost 变换器控制函数（遗留代码）
│   ├── LLC_function.c/h     # LLC 谐振变换器控制（当前使用）
│   ├── led.c/h              # LED GPIO 控制
│   ├── key.c/h              # 矩阵键盘驱动
│   ├── relay.c/h            # 继电器控制
│   ├── exit.c/h             # 外部中断处理程序
│   └── mppt.c/h             # MPPT 算法实现
├── DSP2833x_Librsries/       # TI 外设库
│   ├── DSP2833x_*.c         # 外设驱动源文件
│   ├── *.cmd                # 链接器命令文件
│   └── IQmath.lib           # 定点数学库
└── targetConfigs/           # CCS 目标配置
```

## 架构

### 控制环路架构

固件实现了 LLC 变换器的 **双环控制结构**：

1. **快速环路（Timer0，~50kHz/20us）**：
   - ADC 采样 (`LLC_ADC_Sample()`)
   - 电压 PI 控制器 (`LLC_V_PI_Controller_ComputeIncremental_AntiWindup()`)
   - PWM 频率更新 (`LLC_PWM_Update()`)

2. **慢速环路（Timer1，400Hz/2.5ms）**：
   - 状态机执行 (`LLC_State_M()`)
   - 保护逻辑（OVP、OCP、UVP）
   - 软启动序列

### 状态机

LLC 变换器使用状态机（`LLC_function.h` 中的 `S_State_M` 枚举）：
- `SSInit`：初始化状态
- `SSWait`：等待状态（启动前延迟 1.5 秒）
- `SSRun`：正常运行

遗留的 Boost 变换器有类似的状态（`function.h` 中的 `STATE_M` 枚举）：
- `Init` → `Wait` → `Rise`（软启动）→ `Run` → `Err`

### 关键数据结构

来自 `function.h`：
- `struct _PWM_Values`：PWM 时序参数（频率、占空比、死区时间）
- `struct _ADC_Values`：采样值（Vout、IL、Vin、Iout）
- `struct _Ctr_values`：控制器状态（PI 误差、输出、限幅）
- `struct _FLAG`：系统标志（状态、控制、错误）

### RAM 中的关键函数

标记为 `#pragma CODE_SECTION(fn, ".TI.ramfunc")` 的函数从 RAM 执行以提高速度：
- `Adc_Sample_Process()` / `Process_Valley_Samples()` / `Process_Peak_Samples()`
- `PWM_Update()`、`PWM_Enable()`、`PWM_Disable()`
- `State_M()` 和所有状态处理函数
- `V_PI_Controller_ComputeIncremental_AntiWindup()`
- `I_PI_Controller_ComputeIncremental_AntiWindup()`
- `ShortOff()`、`SwOCP()`、`SwOVP()`、`SwUVP()`

## 硬件配置

### 时钟
- CPU 频率：150 MHz
- ADC 时钟：HSPCLK 分频（参见 `ADC_MODCLK` 定义）

### PWM
- 开关频率：100 kHz（`LLC_function.h` 中定义为 `fs`）
- ePWM1 用于 LLC 半桥驱动
- 死区时间：可通过 `EPwm1Regs.DBRED/DBFED` 配置

### ADC 通道
来自 `adc.c`：
- ADCRESULT0：Vout（ADCINA0）
- ADCRESULT1：IL/电感电流（ADCINA1）
- ADCRESULT2：Vin（ADCINB0）
- ADCRESULT3：Iout（ADCINB1）

缩放系数：`44.4444f`（分压比）

### GPIO 分配
来自 `led.c` 和其他文件：
- LED：GPIO68、GPIO67、GPIO66、GPIO65、GPIO64、GPIO10、GPIO11
- 继电器：GPIO15
- 按键：GPIO12、GPIO13、GPIO14（输入），GPIO48-50（扫描输出）

## 重要说明

### Flash 与 RAM 执行
项目支持 Flash 和 RAM 两种执行模式：
- `function.h` 中定义的 `_FLASHOK` 宏控制 Flash 初始化
- Ramfuncs 段在启动时将代码从 Flash 复制到 RAM

### 双变换器支持
代码库包含对两种变换器类型的支持：
1. **Boost 变换器** (`function.c/h`)：遗留代码，目前在 main 中被注释掉
2. **LLC 谐振变换器** (`LLC_function.c/h`)：当前使用的实现

修改控制逻辑时，请确保编辑的是 LLC 专用文件，而非遗留的 Boost 文件。

### 保护阈值
来自 `function.c`：
- 过压保护（OVP）：150V，延时 100ms
- 欠压保护（UVP）：25V，延时 500ms
- 过流保护（OCP）：15A，延时 500ms
- 短路检测：Iout > 7A 且 Vout < 7.5V

### 包含路径
来自 `.cproject` 的关键包含路径：
- `${PROJECT_ROOT}`
- `${workspace_loc:/${ProjName}/APP/APP_Libraries}`
- `D:\AAAstudy\DSPF28335_Program\DSP2833x_Libraries\DSP2833x_common\include`
- `D:\AAAstudy\DSPF28335_Program\DSP2833x_Libraries\DSP2833x_headers\include`

## 开发工作流程

1. **修改控制参数**：编辑 `LLC_function.c` - 查找 PI 控制器增益、参考值和限幅
2. **添加新外设**：参考 `DSP2833x_Librsries/` 中的现有模式
3. **调试**：使用 LED 翻转（LED1_TOGGLE 到 LED7_TOGGLE 宏）进行实时跟踪
4. **测试保护**：修改 `function.c` 中的阈值常量（搜索 MAX_/MIN_ 定义）

## 常见任务

### 调整 PI 控制器增益
在 `function.c`（Boost）或 `LLC_function.c`（LLC）中编辑：
```c
Boost_Ctr.V_Kp = 0.0736f;
Boost_Ctr.V_Ki = 462.57f;
Boost_Ctr.I_Kp = 0.0566f;
Boost_Ctr.I_Ki = 2488.5f;
```

### 更改 PWM 频率
在 `LLC_function.h` 中编辑：
```c
#define fs      100000.0f       // 开关频率
```

然后重新计算初始化中的 `TBPRD`。

### 更改 LLC 谐振频率
在 `LLC_function.h` 中编辑谐振频率宏定义：
```c
#define LLC_RESONANT_FREQ       400000.0f       // 谐振频率: 400kHz
#define LLC_RESONANT_CYCLE_US   2.5f            // 谐振周期: 2.5us (1/400kHz)
```

谐振频率用于：
- PI 控制器中心工作点 (`LLC_V_PI_Controller_ComputeIncremental_AntiWindup()`)
- 周期时间初始化 (`LLC_ValInit()`)
- PI 控制器清除时的复位值 (`LLC_V_PI_Clear()`)

### 添加新的 ADC 通道
修改 `adc.c` 中的 `ADC_Init()` 并更新通道选择寄存器（`ADCCHSELSEQ1`）。
