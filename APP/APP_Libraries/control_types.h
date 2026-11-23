#ifndef APP_CONTROL_TYPES_H_
#define APP_CONTROL_TYPES_H_

/* Commands shared by the fast and slow control paths. */
struct _Ctr_values
{
    float V_Ref;
    float MAX_CycleTime;   /* Dynamic period limit (us) */
    float MIN_CycleTime;   /* Period limit (us) */
    float Ctrl_CycleTime;  /* Commanded period (us) */
    float Ctrl_DeadTime;   /* Commanded dead time (ns) */
};

struct _ADC_Values
{
    float IL;
    float Iout;
    float Vout;
    float Vin;
    float Iout_Offset;    /* Raw ADC counts */
};

#endif /* APP_CONTROL_TYPES_H_ */
