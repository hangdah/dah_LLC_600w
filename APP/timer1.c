/**
 * @file timer1.c
 * @brief CPU Timer1驱动 - 慢速控制环 (400Hz)
 * @date 2025
 * @author da
 *
 * Timer1配置:
 * - 周期: 2.5ms (400Hz)
 * - 执行状态机和保护逻辑
 * - 在RAM中运行以提高速度
 */

#include "timer1.h"
#include "led.h"
#include "function.h"
#include "LLC_function.h"

#pragma CODE_SECTION(Timer1_IRQn, ".TI.ramfunc")

/**
 * @brief 初始化CPU Timer1
 * @param Freq CPU频率 (MHz)
 * @param Period 定时器周期 (微秒)
 * @return None
 */
void Timer1_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1; /* 使能Timer1时钟 */
    EDIS;

    /* 注册中断处理程序 */
    EALLOW;
    PieVectTable.XINT13 = &Timer1_IRQn;
    EDIS;

    /* 配置Timer1 */
    CpuTimer1.RegsAddr = &CpuTimer1Regs;
    CpuTimer1Regs.PRD.all  = 0xFFFFFFFF;
    CpuTimer1Regs.TPR.all  = 0;
    CpuTimer1Regs.TPRH.all = 0;
    CpuTimer1Regs.TCR.bit.TSS = 1;              /* 停止定时器 */
    CpuTimer1Regs.TCR.bit.TRB = 1;              /* 重载周期 */
    CpuTimer1.InterruptCount = 0;

    ConfigCpuTimer(&CpuTimer1, Freq, Period);

    CpuTimer1Regs.TCR.bit.TSS=0;                /* 启动定时器 */

    /* 使能中断 */
    IER |= M_INT13;

    EINT;
    ERTM;
}

/**
 * @brief Timer1中断服务程序 - 慢速控制环
 * @param None
 * @return None
 *
 * 每2.5ms执行:
 * - 翻转LED2用于调试
 * - 执行LLC状态机
 */
interrupt void Timer1_IRQn(void)
{
    LED2_TOGGLE;
    LLC_State_M();
}
