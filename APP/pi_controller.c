#include "pi_controller.h"

#pragma CODE_SECTION(PI_ComputeIncremental_AntiWindupLimited, ".TI.ramfunc")
float PI_ComputeIncremental_AntiWindupLimited(PIController *pi,
                                            float ref,
                                            float fbk,
                                            float outMin,
                                            float outMax)
{
    float output;

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

    pi->DeltaOut = pi->Kp * (pi->Error1 - pi->Error2) + pi->Ki_Ts * pi->Error1;
    output = pi->Out + pi->DeltaOut;
    if (output > outMax)
        output = outMax;
    else if (output < outMin)
        output = outMin;

    /* Start the next step from the saturated output. */
    pi->Out = output;
    pi->Error2 = pi->Error1;
    return output;
}

#pragma CODE_SECTION(PI_ComputeIncremental_AntiWindup, ".TI.ramfunc")
float PI_ComputeIncremental_AntiWindup(PIController *pi, float ref, float fbk)
{
    return PI_ComputeIncremental_AntiWindupLimited(pi, ref, fbk, pi->OutMin, pi->OutMax);
}
