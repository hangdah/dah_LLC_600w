/**
 * @file main.c
 * @brief LLC谐振变换器控制主入口
 * @date 2025
 * @author da
 *
 * 本项目为基于TI TMS320F28335 DSP的微型逆变器应用实现控制软件。
 * Timer0 执行电压 PI 调频控制（100 kHz），Timer1 执行状态机（400 Hz）。
 */

#include "adc.h"
#include "timer0.h"
#include "timer1.h"
#include "epwm.h"
#include "led.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "control_config.h"
#include "llc_app.h"
#include <stdint.h>

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadSize;
extern Uint16 RamfuncsRunStart;

/**
 * @brief 主函数 - 系统初始化与主循环
 * @param None
 * @return None
 */
void main()
{
    /* 为提高执行速度，将代码从Flash复制到RAM */
    #ifdef _FLASHOK
    memcpy((uint16_t *)&RamfuncsRunStart,(uint16_t *)&RamfuncsLoadStart,(unsigned long)&RamfuncsLoadSize);
    #endif

    /* 初始化系统控制：PLL、看门狗、时钟 */
    InitSysCtrl();

    /* 如果从Flash运行，则初始化Flash */
    #ifdef _FLASHOK
    InitFlash();
    #endif

    /* 初始化期间禁用全局中断 */
    DINT;

    /* 初始化PIE控制并清除中断寄存器 */
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    EALLOW;
    PieCtrlRegs.PIEACK.all = 0xFFFF;
    EDIS;

    /* 配置期间禁用PWM时钟同步 */
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    /* 初始化外设 */
    LED_Init();     /* 初始化用于LED的GPIO */
    LLC_App_Init(); /* 初始化LLC控制参数和PWM时序值 */
    ADC_Init();     /* 初始化ADC用于电压/电流采样 */
    LLC_App_DisableOutput();                        /* 在启动序列完成前保持PWM禁用 */
    EPWM1_Init(LLC_App_GetInitialTbprd());          /* 使用计算出的周期初始化ePWM1 */
    Timer0_Init(LLC_CPU_MHZ, LLC_FAST_PERIOD_US);   /* Timer0: 10us (100kHz) 用于快速控制环路 */
    Timer1_Init(LLC_CPU_MHZ, LLC_SLOW_PERIOD_US);   /* Timer1: 2500us (400Hz) 用于慢速控制环路和状态机 */

    /* 启用PWM时钟同步 */
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

    /* 启用全局中断和实时调试 */
    EINT;   /* 启用全局中断 */
    ERTM;   /* 启用实时调试模式 */

    /* 主循环 - 控制逻辑在中断处理程序中运行 */
    while(1)
    {
    }
}
