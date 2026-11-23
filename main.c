/**
 * @file main.c
 * @brief LLC谐振变换器控制主入口
 * @date 2025
 * @author da
 *
 * 本项目为基于TI TMS320F28335 DSP的微型逆变器应用实现控制软件。
 * 其特点是双闭环控制结构，包含用于电压PI控制的快速环路（Timer0 ~50kHz）
 * 和用于状态机及保护逻辑的慢速环路（Timer1 400Hz）。
 */

#include "adc.h"
#include "timer0.h"
#include "timer1.h"
#include "epwm.h"
#include "led.h"
#include "relay.h"
#include "mppt.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "function.h"
#include "LLC_function.h"
#include <stdint.h>

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadSize;
extern Uint16 RamfuncsRunStart;

extern struct _PWM_Values Boost_PWM;
extern struct _PWM_Values LLC_PWM;

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
    LED_Init();                     /* 初始化用于LED的GPIO */
    LLC_Ctr_Init();                  /* 初始化PI控制器参数 */
    LLC_PWM_Values_Init();          /* 初始化PWM时序值 */

    /* 初始化定时器 - Timer0: 10us (100kHz) 用于快速控制环路 */
    Timer0_Init(150.0f, 10.0f);     /* CPU频率: 150MHz, 周期: 10us */
    /* Timer1: 2500us (400Hz) 用于慢速控制环路和状态机 */
    Timer1_Init(150.0f, 2500.0f);   /* CPU频率: 150MHz, 周期: 2500us */

    EPWM1_Init(LLC_PWM.PWM_TBPRD); /* 使用计算出的周期初始化ePWM1 */
    ADC_Init();                     /* 初始化ADC用于电压/电流采样 */
    PWM_Disable();                  /* 在启动序列完成前保持PWM禁用 */

    /* 启用PWM时钟同步 */
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

    /* 启用全局中断和实时调试 */
    EINT;                           /* 启用全局中断 */
    ERTM;                           /* 启用实时调试模式 */

    /* 主循环 - 控制逻辑在中断处理程序中运行 */
    while(1)
    {
        /* 空闲循环 - 所有处理都在Timer0/Timer1的中断服务程序中完成 */
    }
}
