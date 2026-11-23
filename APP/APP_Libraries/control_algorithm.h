#ifndef APP_CONTROL_ALGORITHM_H_
#define APP_CONTROL_ALGORITHM_H_

#include "control_types.h"

/* 通用增量式 PI 的参数、限幅和运行状态。 */
typedef struct
{
    float Kp, Ki, Ki_Ts;    /* 比例增益、积分增益、积分增益与采样周期的乘积 */
    float Error1, Error2;   /* 当前误差与上一拍误差 */
    float Out, DeltaOut;    /* 累加输出与本拍增量 */
    float OutMax, OutMin;   /* 默认输出上下限 */
    float ErrMax, ErrMin;   /* 误差上下限同时为 0 时跳过误差限幅 */
} PIController;

/* 临时上下限仅用于本拍运算，保留控制器的默认限幅配置。 */
float PI_ComputeIncremental_AntiWindupLimited(PIController *pi,
                                            float ref,
                                            float fbk,
                                            float outMin,
                                            float outMax);

/* 清除 LLC 电压 PI 的误差历史和累加输出。 */
void LLC_V_PI_Clear(void);
/* 根据滤波后的反馈值更新 LLC 开关周期指令，单位为 us。 */
void LLC_V_PI_Update(float vout, volatile struct _Ctr_values *control);

#endif /* APP_CONTROL_ALGORITHM_H_ */
