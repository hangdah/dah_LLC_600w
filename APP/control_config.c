#include "control_config.h"

/* 将 12 位 ADC 原始计数换算为采样端电压，满量程为 3.0 V。 */
const float ADC_Transform = 3.0f / 4095.0f;

/* 初始化控制指令。软启动状态机随后会调整参考值、周期上限和死区。 */
void LLC_ControlValues_Init(volatile struct _Ctr_values *control)
{
    control->V_Ref = LLC_INITIAL_VREF;
    control->MAX_CycleTime = LLC_MAX_CYCLE_US;
    control->MIN_CycleTime = LLC_MIN_CYCLE_US;
    control->Ctrl_DeadTime = LLC_MAX_DEAD_TIME_NS;
    control->Ctrl_CycleTime = LLC_MIN_CYCLE_US;
}
