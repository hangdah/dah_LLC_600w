/**
 * @file adc.c
 * @brief 用于电压和电流采样的ADC驱动
 * @date 2025
 * @author da
 *
 * ADC配置:
 * - SEQ1: 由 LLC 快环软件触发
 * - 通道: ADCINA0 (Vout), ADCINA1 (IL), ADCINB0 (Vin), ADCINB1 (Iout)
 * - 双序列器模式，用于独立操作
 */

#include "adc.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

/**
 * @brief 初始化ADC模块
 * @param None
 * @return None
 *
 * 配置ADC为双序列器模式，SEQ1 使用软件触发。
 */
void ADC_Init(void)
{
    EALLOW;

    /* ADC基础初始化/校准 */
    InitAdc();

    /* 使能ADC时钟 */
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;

    /* ADC时钟分频 */
    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_MODCLK;

    /* 工作模式：双序列器 + 单次序列 + 顺序采样 */
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 0;     // 双序列器模式，使用SEQ1
    AdcRegs.ADCTRL1.bit.CONT_RUN = 0;     // Start/Stop模式
    AdcRegs.ADCTRL1.bit.ACQ_PS   = 6;     // 采样窗 = (6+1) * ADCCLK
    AdcRegs.ADCTRL1.bit.CPS      = 0;     // 按你的时钟设计保留/调整

    AdcRegs.ADCTRL3.bit.SMODE_SEL = 0;    // 0 = 顺序采样，1 = 同时采样

    /* 不用ePWM触发，不用外部XINT2触发 */
    AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 0;
    AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1   = 0;

    /* 不用ADC中断进CPU，但保留SEQ1完成标志用于轮询 */
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 0;
    AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 0; // 每次SEQ1结束都置INT_SEQ1

    /* SEQ1共4次转换：CONV00~CONV03 */
    AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x3;

    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;  // ADCINA0 -> Vout
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;  // ADCINA1 -> IL
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x8;  // ADCINB0 -> Vin
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x9;  // ADCINB1 -> Iout

    /* 清一次完成标志 */
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;

    EDIS;
}
