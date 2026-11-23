/**
 * @file led.c
 * @brief LED GPIO控制驱动
 * @date 2025
 * @author da
 *
 * LED映射:
 * - LED1-5: GPIO64-68 (端口C)
 * - LED6-7: GPIO10-11 (端口A)
 */

#include "led.h"

/**
 * @brief 初始化所有LED GPIO
 * @param None
 * @return None
 *
 * 将LED配置为带使能上拉的输出。
 * 所有LED初始状态为ON (低电平有效)。
 */
void LED_Init(void){

    EALLOW; /* 禁用写保护 */
    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK=1; /* 使能GPIO时钟 */

    /* LED1 - GPIO68 */
    GpioCtrlRegs.GPCMUX1.bit.GPIO68=0;     /* 配置为GPIO */
    GpioCtrlRegs.GPCDIR.bit.GPIO68=1;      /* 设置为输出 */
    GpioCtrlRegs.GPCPUD.bit.GPIO68=0;      /* 使能上拉 */
    GpioDataRegs.GPCSET.bit.GPIO68=1;      /* 置高 (LED灭) */

    /* LED2 - GPIO67 */
    GpioCtrlRegs.GPCMUX1.bit.GPIO67=0;
    GpioCtrlRegs.GPCDIR.bit.GPIO67=1;
    GpioCtrlRegs.GPCPUD.bit.GPIO67=0;
    GpioDataRegs.GPCSET.bit.GPIO67=1;

    /* LED3 - GPIO66 */
    GpioCtrlRegs.GPCMUX1.bit.GPIO66=0;
    GpioCtrlRegs.GPCDIR.bit.GPIO66=1;
    GpioCtrlRegs.GPCPUD.bit.GPIO66=0;
    GpioDataRegs.GPCSET.bit.GPIO66=1;

    /* LED4 - GPIO65 */
    GpioCtrlRegs.GPCMUX1.bit.GPIO65=0;
    GpioCtrlRegs.GPCDIR.bit.GPIO65=1;
    GpioCtrlRegs.GPCPUD.bit.GPIO65=0;
    GpioDataRegs.GPCSET.bit.GPIO65=1;

    /* LED5 - GPIO64 */
    GpioCtrlRegs.GPCMUX1.bit.GPIO64=0;
    GpioCtrlRegs.GPCDIR.bit.GPIO64=1;
    GpioCtrlRegs.GPCPUD.bit.GPIO64=0;
    GpioDataRegs.GPCSET.bit.GPIO64=1;

    /* LED6 - GPIO10 */
    GpioCtrlRegs.GPAMUX1.bit.GPIO10=0;
    GpioCtrlRegs.GPADIR.bit.GPIO10=1;
    GpioCtrlRegs.GPAPUD.bit.GPIO10=0;
    GpioDataRegs.GPASET.bit.GPIO10=1;

    /* LED7 - GPIO11 */
    GpioCtrlRegs.GPAMUX1.bit.GPIO11=0;
    GpioCtrlRegs.GPADIR.bit.GPIO11=1;
    GpioCtrlRegs.GPAPUD.bit.GPIO11=0;
    GpioDataRegs.GPASET.bit.GPIO11=1;

    EDIS; /* 使能写保护 */
}
