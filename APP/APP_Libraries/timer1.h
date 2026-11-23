/**
 * @file timer1.h
 * @brief 慢速控制环的CPU Timer1头文件
 * @date 2025
 * @author da
 *
 * Timer1配置:
 * - 周期: 2.5ms (400Hz)
 * - 用于: 状态机, 保护逻辑, 软启动
 * - 优先级: 低于Timer0
 */

#ifndef APP_APP_LIBRARIES_TIMER1_H_
#define APP_APP_LIBRARIES_TIMER1_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"


void Timer1_Init(float Freq, float Period);
interrupt void Timer1_IRQn(void);



#endif /* APP_APP_LIBRARIES_TIMER1_H_ */
