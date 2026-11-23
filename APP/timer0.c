/**
 * @file timer0.c
 * @brief CPU Timer0驱动 - 快速控制环 (~50kHz)
 * @date 2025
 * @author da
 *
 * Timer0配置:
 * - 周期: 20us (50kHz)
 * - 每20us触发一次用于电压PI控制和PWM更新
 * - 在RAM中执行以提高速度
 */

#include "timer0.h"
#include "function.h"
#include "LLC_function.h"
#include "led.h"

/**
 * @brief 初始化CPU Timer0
 * @param Freq CPU频率 (MHz)
 * @param Period 定时器周期 (微秒)
 * @return None
 */
void Timer0_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; /* 使能Timer0时钟 */
    EDIS;

    /* 注册中断处理程序 */
    EALLOW;
    PieVectTable.TINT0 = &Timer0_IRQn;
    EDIS;

    /* 配置Timer0 */
    CpuTimer0.RegsAddr = &CpuTimer0Regs;
    CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
    CpuTimer0Regs.TPR.all  = 0;
    CpuTimer0Regs.TPRH.all = 0;
    CpuTimer0Regs.TCR.bit.TSS = 1;              /* 停止定时器 */
    CpuTimer0Regs.TCR.bit.TRB = 1;              /* 重载周期 */
    CpuTimer0.InterruptCount = 0;
    ConfigCpuTimer(&CpuTimer0, Freq, Period);

    CpuTimer0Regs.TCR.bit.TSS=0;                /* 启动定时器 */

    /* 使能中断 */
    IER |= M_INT1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    EINT;
    ERTM;
}

/**
 * @brief Timer0中断服务程序 - 快速控制环
 * @param None
 * @return None
 *
 * 每20us执行:
 * 1. 翻转LED1用于调试
 * 2. 采样ADC值
 * 3. 执行电压PI控制器
 * 4. 更新PWM频率
 */
interrupt void Timer0_IRQn(void)
{
    LED1_TOGGLE;

    /* 先读上次结果，再启动下次ADC */
    LLC_ADC_ReadPrevious_And_StartNext();

    /* 至少有一帧有效ADC数据后再进入控制 */
    if (g_llc_adc_valid)
    {
        LLC_V_PI_Controller_ComputeIncremental_AntiWindup();
        LLC_PWM_Update();
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
