#ifndef APP_PI_CONTROLLER_H_
#define APP_PI_CONTROLLER_H_

typedef struct
{
    float Kp, Ki, Ki_Ts;    /* Ki_Ts is Ki multiplied by the sample period. */
    float Error1, Error2;   /* Current and previous error. */
    float Out, DeltaOut;    /* Accumulated and incremental output. */
    float OutMax, OutMin;
    float ErrMax, ErrMin;   /* Two zero limits disable error clamping. */
} PIController;

float PI_ComputeIncremental_AntiWindup(PIController *pi, float ref, float fbk);

/* Override the output limits for this step without changing OutMin or OutMax. */
float PI_ComputeIncremental_AntiWindupLimited(PIController *pi,
                                            float ref,
                                            float fbk,
                                            float outMin,
                                            float outMax);

#endif /* APP_PI_CONTROLLER_H_ */
