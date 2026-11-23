/**
 * @file exit.c
 * @brief 用于键盘的外部中断驱动
 * @date 2025
 * @author da
 *
 * 外部中断配置:
 * - XINT1: GPIO12 (按键第1列)
 * - XINT2: GPIO13 (按键第2列)
 * - 下降沿触发带消抖
 */

#include "exit.h"
#include "key.h"
#include "led.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

/**
 * @brief 初始化外部中断1
 * @param None
 * @return None
 */
void EXTI1_Init(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;    /* 使能GPIO时钟 */
    EDIS;

    EALLOW;
    /* 配置按键端口 */
    GpioCtrlRegs.GPAMUX1.bit.GPIO12=0;
    GpioCtrlRegs.GPADIR.bit.GPIO12=0;
    GpioCtrlRegs.GPAPUD.bit.GPIO12=0;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 0;       /* XINT1同步到SYSCLKOUT */

    GpioCtrlRegs.GPBMUX2.bit.GPIO48=0;
    GpioCtrlRegs.GPBDIR.bit.GPIO48=1;
    GpioCtrlRegs.GPBPUD.bit.GPIO48=0;
    GpioDataRegs.GPBCLEAR.bit.GPIO48=1;
    EDIS;

    EALLOW;
    GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 12;  /* XINT1为GPIO12 */
    EDIS;

    EALLOW;
    PieVectTable.XINT1 = &EXTI1_IRQn;
    EDIS;

    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          /* 使能PIE组1 INT4 */

    XIntruptRegs.XINT1CR.bit.POLARITY = 0;      /* 下降沿触发 */
    XIntruptRegs.XINT1CR.bit.ENABLE= 1;         /* 使能XINT1 */

    IER |= M_INT1;                              /* 使能CPU INT1 */
    EINT;                                       /* 使能全局中断 */
    ERTM;
}

/**
 * @brief 外部中断1 ISR
 * @param None
 * @return None
 */
interrupt void EXTI1_IRQn(void)
{
    Uint32 i;
    for(i=0;i<10000;i++);    /* 键盘消抖延时 */
    while(!KEY_H1);
    LED2_TOGGLE;
    PieCtrlRegs.PIEACK.bit.ACK1=1;
}

/**
 * @brief 初始化外部中断2
 * @param None
 * @return None
 */
void EXTI2_Init(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;    /* 使能GPIO时钟 */
    EDIS;

    EALLOW;
    /* 配置按键端口 */
    GpioCtrlRegs.GPAMUX1.bit.GPIO13=0;
    GpioCtrlRegs.GPADIR.bit.GPIO13=0;
    GpioCtrlRegs.GPAPUD.bit.GPIO13=0;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 2;        /* XINT2带6采样输入限定 */
    GpioCtrlRegs.GPACTRL.bit.QUALPRD1 = 0xFF;   /* 输入限定周期: 510*SYSCLKOUT */

    GpioCtrlRegs.GPBMUX2.bit.GPIO48=0;
    GpioCtrlRegs.GPBDIR.bit.GPIO48=1;
    GpioCtrlRegs.GPBPUD.bit.GPIO48=0;
    GpioDataRegs.GPBCLEAR.bit.GPIO48=1;
    EDIS;

    EALLOW;
    GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 13;  /* XINT2为GPIO13 */
    EDIS;

    EALLOW;
    PieVectTable.XINT2 = &EXTI2_IRQn;
    EDIS;

    PieCtrlRegs.PIEIER1.bit.INTx5 = 1;          /* 使能PIE组1 INT5 */

    XIntruptRegs.XINT2CR.bit.POLARITY = 0;      /* 下降沿触发 */
    XIntruptRegs.XINT2CR.bit.ENABLE = 1;        /* 使能XINT2 */

    IER |= M_INT1;                              /* 使能CPU INT1 */
    EINT;                                       /* 使能全局中断 */
    ERTM;
}

/**
 * @brief 外部中断2 ISR
 * @param None
 * @return None
 */
interrupt void EXTI2_IRQn(void)
{
    Uint32 i;
    for(i=0;i<10000;i++);    /* 键盘消抖延时 */
    while(!KEY_H2);
    LED3_TOGGLE;
    PieCtrlRegs.PIEACK.bit.ACK1=1;
}
