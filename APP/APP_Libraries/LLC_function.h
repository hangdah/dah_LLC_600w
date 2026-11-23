/**
 * @file LLC_function.h
 * @brief LLC谐振变换器控制函数头文件
 * @date 2025
 * @author da
 *
 * 本文件包含LLC谐振变换器控制函数的定义和声明，
 * 包括PI控制器、状态机和保护功能。
 */

#ifndef APP_APP_LIBRARIES_LLC_FUNCTION_H_
#define APP_APP_LIBRARIES_LLC_FUNCTION_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

extern volatile Uint16 g_llc_adc_started;
extern volatile Uint16 g_llc_adc_valid;

#define fs      100000.0f       /* 开关频率 */
#define f_CPU   150000000.0f    /* CPU频率 */

/* 谐振频率定义 */
#define LLC_RESONANT_FREQ       400000.0f       /* 谐振频率: 400kHz */
#define LLC_RESONANT_CYCLE_US   2.5f            /* 谐振周期: 2.5us (1/400kHz) */

/***************************** 控制标志定义 *****************/
#define     F_OUT_EN        0x0001  /* 输出使能 */
#define     F_BURST         0x0002  /* Burst模式使能 */

#define     clrRegBits(reg, mask)   (reg &= ~(unsigned int)(mask))
#define     getRegBits(reg, mask)   (reg & (unsigned int)(mask))

/***************************** 错误类型定义 *****************/
#define     LLC_Error_NOERR         0x0000  /* 无错误 */
#define     LLC_Error_OVP           0x0001  /* 输出过压 */
#define     LLC_Error_OCP           0x0002  /* 输出过流 */
#define     LLC_Error_OUT_SHORT     0x0004  /* 输出短路 */
#define     LLC_Error_UVP           0x0008  /* 输出欠压 */

/** 软启动状态枚举 */
typedef enum
{
    SSInit,     /* 软启动初始化 */
    SSWait,     /* 软启动等待 */
    SSRun       /* 软启动运行 */
 } S_State_M;

void LLC_Ctr_Init(void);
void LLC_PWM_Values_Init(void);
void LLC_ValInit(void);
void LLC_PWM_Update(void);
void LLC_ADC_ReadPrevious_And_StartNext(void);
char IoutOffsetCal(void);

void LLC_V_PI_Controller_ComputeIncremental_AntiWindup(void);
void LLC_I_PI_Controller_ComputeIncremental_AntiWindup(void);
void LLC_State_M(void);
void LLC_StateMInit(void);
void LLC_StateMWait(void);
void LLC_StateMRise(void);
void LLC_StateMRun(void);
void LLC_StateMErr(void);
void LLC_V_PI_Clear(void);
void LLC_BurstCtl(void);

#endif /* APP_APP_LIBRARIES_LLC_FUNCTION_H_ */
