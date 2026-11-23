#include "llc_app.h"
#include "control_algorithm.h"
#include "control_config.h"
#include "control_pwm.h"
#include "control_sampling.h"
#include "control_state.h"

/* Timer0 and Timer1 both access these commands. Keep the existing update order. */
static volatile struct _Ctr_values LLC_Ctr;

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
void LLC_App_SlowTick(void)
{
    LLC_State_M(&LLC_Ctr);
}
