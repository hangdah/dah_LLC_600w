/**
 * @file exit.h
 * @brief 外部中断驱动头文件
 * @date 2025
 * @author da
 *
 * 外部中断配置:
 * - XINT1: GPIO12 (按键第1列)
 * - XINT2: GPIO13 (按键第2列)
 * - 下降沿触发
 */

#ifndef APP_APP_LIBRARIES_EXIT_H_
#define APP_APP_LIBRARIES_EXIT_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

void EXTI1_Init(void);
interrupt void EXTI1_IRQn(void);
void EXTI2_Init(void);
interrupt void EXTI2_IRQn(void);



#endif /* APP_APP_LIBRARIES_EXIT_H_ */
