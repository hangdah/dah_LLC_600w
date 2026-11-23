#include "control_config.h"

const float ADC_Transform = 3.0f / 4095.0f;

void LLC_ControlValues_Init(volatile struct _Ctr_values *control)
{
    control->V_Ref = LLC_INITIAL_VREF;
    control->MAX_CycleTime = LLC_MAX_CYCLE_US;
    control->MIN_CycleTime = LLC_MIN_CYCLE_US;
    control->Ctrl_DeadTime = LLC_MAX_DEAD_TIME_NS;
    control->Ctrl_CycleTime = LLC_MIN_CYCLE_US;
}
