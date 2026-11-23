#include "llc_app.h"
#include "control_algorithm.h"
#include "control_config.h"
#include "control_pwm.h"
#include "control_sampling.h"
#include "control_state.h"

/* Timer0 快环和 Timer1 慢环共用的控制指令，按现有中断顺序更新。 */
static volatile struct _Ctr_values LLC_Ctr;

/* 在外设初始化期间清除 PI 状态，并准备控制指令和初始 PWM 时序。 */
void LLC_App_Init(void)
{
    LLC_V_PI_Clear();
    LLC_ControlValues_Init(&LLC_Ctr);
    LLC_PWM_Values_Init();
}

Uint16 LLC_App_GetInitialTbprd(void)
{
    return LLC_PWM_GetTbprd();
}

void LLC_App_DisableOutput(void)
{
    PWM_Disable();
}

#pragma CODE_SECTION(LLC_App_FastTick, ".TI.ramfunc")
/* 每 10 us 处理采样、执行电压 PI 并更新 PWM，首帧有效后才运行 PI。 */
void LLC_App_FastTick(void)
{
    const struct _ADC_Values *values = LLC_ADC_ReadPrevious_And_StartNext();

    if (values != 0)
    {
        LLC_V_PI_Update(values->Vout, &LLC_Ctr);
        LLC_PWM_Update(&LLC_Ctr);
    }
}

#pragma CODE_SECTION(LLC_App_SlowTick, ".TI.ramfunc")
/* 每 2.5 ms 推进状态机，处理软启动和故障状态。 */
void LLC_App_SlowTick(void)
{
    LLC_State_M(&LLC_Ctr);
}
