#ifndef APP_CONTROL_CONFIG_H_
#define APP_CONTROL_CONFIG_H_

#include "control_types.h"

/* Flash 启动配置、CPU 时钟（MHz）及快慢环周期（us）。 */
#define _FLASHOK                   1
#define LLC_CPU_MHZ                150.0f
#define LLC_FAST_PERIOD_US         10.0f
#define LLC_SLOW_PERIOD_US         2500.0f
#define LLC_CONTROL_RATE_HZ        (1000000.0f / LLC_FAST_PERIOD_US)

/* 谐振频率（Hz）、开关周期（us）和死区时间（ns）。 */
#define LLC_RESONANT_FREQ          400000.0f
#define LLC_RESONANT_CYCLE_US      2.5f
#define LLC_MIN_CYCLE_US           1.0f
#define LLC_MAX_CYCLE_US           3.33f
#define LLC_MIN_DEAD_TIME_NS       50.0f
#define LLC_MAX_DEAD_TIME_NS       300.0f
#define LLC_INITIAL_DEAD_TIME_NS   20.0f
#define LLC_INITIAL_DUTY           0.1f
/* ePWM1 使用上下计数且时基不分频，据此换算周期和死区寄存器计数。 */
#define LLC_TBPRD_PER_US           (LLC_CPU_MHZ * 0.5f)
#define LLC_DEAD_COUNTS_PER_NS     (LLC_CPU_MHZ / 1000.0f)

#define LLC_INITIAL_VREF           400.0f
#define LLC_SOFT_START_VREF        100.0f
#define LLC_V_KP                   150.0f
#define LLC_V_KI                   150000.0f
#define LLC_V_KI_TS                (LLC_V_KI * (1.0f / LLC_CONTROL_RATE_HZ))
#define LLC_V_Error_MAX            20.0f
#define LLC_V_Error_MIN            (-20.0f)
/* PI 的输出限幅为相对谐振周期的偏移量，单位为 us。 */
#define LLC_V_PIOUT_MAX_CYCLE      1.5f
#define LLC_V_PIOUT_MIN_CYCLE      (-0.83f)

#define LLC_WAIT_TICKS             600U
/* 软启动等待计数超过此值时使能 PWM。 */
#define LLC_MAX_Rise_Cnt           200U
#define LLC_SOFT_START_STEP_US     0.0029125f
#define LLC_SOFT_START_STEP_NS     0.275f
#define BURST_ON_CYCLE_THRESHOLD   1.4f
#define BURST_OFF_CYCLE_THRESHOLD  1.6f
#define LLC_BURST_TICKS            8U

/* ADC 滤波、输入换算和零偏校准参数。 */
#define IIR_ALPHA                  0.001f
#define LLC_ADC_INPUT_SCALE        44.4444f
#define LLC_ADC_OFFSET_SHIFT       8
#define LLC_ADC_OFFSET_SAMPLES     (1 << LLC_ADC_OFFSET_SHIFT)

extern const float ADC_Transform;

void LLC_ControlValues_Init(volatile struct _Ctr_values *control);

#endif /* APP_CONTROL_CONFIG_H_ */
