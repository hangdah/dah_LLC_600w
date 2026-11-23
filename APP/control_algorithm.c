#include "control_algorithm.h"
#include "control_config.h"

/* LLC 电压环使用的 PI 实例，增益和限幅取自控制参数。 */
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

#pragma CODE_SECTION(PI_ComputeIncremental_AntiWindupLimited, ".TI.ramfunc")
/* 执行一次增量 PI，临时输出限幅通过参数传入。 */
float PI_ComputeIncremental_AntiWindupLimited(PIController *pi,
                                            float ref,
                                            float fbk,
                                            float outMin,
                                            float outMax)
{
    float output;

    /* 上下限反转时将输出范围收敛到给定上限。 */
    if (outMin > outMax)
        outMin = outMax;

    pi->Error1 = ref - fbk;
    if (pi->ErrMax != 0.0f || pi->ErrMin != 0.0f)
    {
        if (pi->Error1 > pi->ErrMax)
            pi->Error1 = pi->ErrMax;
        else if (pi->Error1 < pi->ErrMin)
            pi->Error1 = pi->ErrMin;
    }

    /* 比例项使用误差差分，积分项使用预乘采样周期的增益。 */
    pi->DeltaOut = pi->Kp * (pi->Error1 - pi->Error2) + pi->Ki_Ts * pi->Error1;
    output = pi->Out + pi->DeltaOut;
    if (output > outMax)
        output = outMax;
    else if (output < outMin)
        output = outMin;

    /* 下一拍从限幅后的输出继续累加，避免状态在饱和时持续增长。 */
    pi->Out = output;
    pi->Error2 = pi->Error1;
    return output;
}

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
 * @param vout 滤波后的输出电压反馈值
 * @param control LLC 电压参考值、周期上下限和周期指令
 * @return None
 */
void LLC_V_PI_Update(float vout, volatile struct _Ctr_values *control)
{
    const float u_sat = PI_ComputeIncremental_AntiWindupLimited(&LLC_PI,
                                                               control->V_Ref,
                                                               vout,
                                                               LLC_PI.OutMin,
                                                               LLC_PI.OutMax);

    /* 将 PI 输出的偏移量换算为开关周期，单位为 us。 */
    control->Ctrl_CycleTime = LLC_RESONANT_CYCLE_US - u_sat;

    /* 更新 PI 状态后，再应用软启动期间动态变化的周期限幅。 */
    if (control->Ctrl_CycleTime > control->MAX_CycleTime)
        control->Ctrl_CycleTime = control->MAX_CycleTime;
    else if (control->Ctrl_CycleTime < control->MIN_CycleTime)
        control->Ctrl_CycleTime = control->MIN_CycleTime;
}
