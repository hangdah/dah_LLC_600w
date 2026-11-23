/**
 * @file relay.c
 * @brief 继电器控制驱动
 * @date 2025
 * @author da
 *
 * 继电器连接到GPIO15
 */

#include "relay.h"

/**
 * @brief 初始化继电器GPIO
 * @param None
 * @return None
 */
void Relay_Init(){
    EALLOW;

    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK=1; /* 使能GPIO时钟 */

    /* 配置GPIO15为输出 */
    GpioCtrlRegs.GPAMUX1.bit.GPIO15=0;
    GpioCtrlRegs.GPAPUD.bit.GPIO15=0;      /* 使能上拉 */
    GpioCtrlRegs.GPADIR.bit.GPIO15=1;      /* 设置为输出 */

    EDIS;

    /* 初始化为OFF状态 */
    GpioDataRegs.GPACLEAR.bit.GPIO15=1;
}
