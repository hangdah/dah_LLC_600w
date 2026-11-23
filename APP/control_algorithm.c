#include "control_algorithm.h"
#include "control_config.h"
#include "pi_controller.h"

static PIController LLC_PI =
{
    .Kp    = LLC_V_KP,
    .Ki    = LLC_V_KI,
    .Ki_Ts = LLC_V_KI_TS,
    .OutMin = LLC_V_PIOUT_MIN_CYCLE,
    .OutMax = LLC_V_PIOUT_MAX_CYCLE,
    .ErrMin = LLC_V_Error_MIN,
    .ErrMax = LLC_V_Error_MAX
};

#pragma CODE_SECTION(LLC_V_PI_Clear, ".TI.ramfunc")
/**
 * @brief 清除电压PI控制器状态
 * @param None
 * @return None
 */
void LLC_V_PI_Clear(void)
{
    LLC_PI.DeltaOut = 0.0f;
    LLC_PI.Error1   = 0.0f;
    LLC_PI.Error2   = 0.0f;
    LLC_PI.Out      = 0.0f;
}

#pragma CODE_SECTION(LLC_V_PI_Update, ".TI.ramfunc")
/**
 * @brief 带抗饱和的LLC电压PI控制器
 * @param vout Filtered feedback value
 * @param control LLC reference, period limits and command
 * @return None
 */
void LLC_V_PI_Update(float vout, volatile struct _Ctr_values *control)
{
    const float u_sat = PI_ComputeIncremental_AntiWindupLimited(&LLC_PI,
                                                               control->V_Ref,
                                                               vout,
                                                               LLC_PI.OutMin,
                                                               LLC_PI.OutMax);

    /* Convert the PI offset to a switching period in us. */
    control->Ctrl_CycleTime = LLC_RESONANT_CYCLE_US - u_sat;

    /* Apply the current soft-start period limit after updating the PI state. */
    if (control->Ctrl_CycleTime > control->MAX_CycleTime)
        control->Ctrl_CycleTime = control->MAX_CycleTime;
    else if (control->Ctrl_CycleTime < control->MIN_CycleTime)
        control->Ctrl_CycleTime = control->MIN_CycleTime;
}
