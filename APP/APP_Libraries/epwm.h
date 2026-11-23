/**
 * @file epwm.h
 * @brief LLC半桥控制的ePWM驱动头文件
 * @date 2025
 * @author da
 *
 * ePWM1 LLC配置:
 * - 增减计数模式 (对称PWM)
 * - 双输出: EPWM1A (上管), EPWM1B (下管)
 * - 可编程死区时间插入
 * - 故障处理的触发区保护
 */

#ifndef APP_APP_LIBRARIES_EPWM_H_
#define APP_APP_LIBRARIES_EPWM_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#define PWM_CH 7

void EPWM1_Init(Uint16 tbprd);
interrupt void epwm1_isr(void);
void HRPWM1_SFO_Init(void);
void EPWM1_SetCompareHR(Uint16 cmpa, float cmpa_frac);

void EPwm1A_SetCompare(Uint16 val);
void EPwm1B_SetCompare(Uint16 val);



#endif /* APP_APP_LIBRARIES_EPWM_H_ */
