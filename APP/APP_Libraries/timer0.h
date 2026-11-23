/**
 * @file timer0.h
 * @brief 快速控制环的CPU Timer0头文件
 * @date 2025
 * @author da
 *
 * Timer0配置:
 * - 周期: 20us (50kHz)
 * - 用于: ADC采样, 电压PI控制, PWM更新
 * - 优先级: 高 (快速控制环)
 */

#ifndef APP_APP_LIBRARIES_TIMER0_H_
#define APP_APP_LIBRARIES_TIMER0_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

void Timer0_Init(float Freq, float Period);
interrupt void Timer0_IRQn(void);



#endif /* APP_APP_LIBRARIES_TIMER0_H_ */
