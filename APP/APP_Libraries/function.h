/**
 * @file function.h
 * @brief Boost变换器控制函数头文件 (旧版本)
 * @date 2025
 * @author da
 *
 * 本文件包含旧版Boost变换器控制函数的定义和声明。
 * 对于新设计，请使用LLC_function.h进行LLC谐振变换器控制。
 */

#ifndef APP_APP_LIBRARIES_FUNCTION_H_
#define APP_APP_LIBRARIES_FUNCTION_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "epwm.h"
#include "led.h"
#include "LLC_function.h"

#define _FLASHOK        1

/** IIR滤波器系数 - 值越小 = 更平滑但响应更慢 */
/** IIR_ALPHA = (2*pi*f_cutoff)/f_sampling */
#define IIR_ALPHA 0.001f

/***************************** 错误定义 *****************/
#define     Error_NOERR         0x0000  /* 无错误 */
#define     Error_OVP           0x0001  /* 输出过压 */
#define     Error_OCP           0x0002  /* 输出过流 */
#define     Error_OUT_SHORT     0x0004  /* 输出短路 */
#define     Error_UVP           0x0008  /* 输出欠压 */

#define setRegBits(reg, mask)               (reg |= (unsigned int)(mask))

/***************************** 状态机枚举 *****************/
typedef enum
{
    Init,   /* 初始化 */
    Wait,   /* 上电等待 */
    Rise,   /* 软启动 */
    Run,    /* 正常运行 */
    Err     /* 错误 */
}STATE_M;

/***************************** 标志结构 *****************/
struct _FLAG
{
    unsigned int    StateM; /* 状态机标志 */
    unsigned int    Ctr;    /* 控制标志 */
    unsigned int    Error;  /* 错误标志 */
};

/***************************** PWM值结构 *****************/
struct _PWM_Values
{
    /* 规定1：任何想要对PWM发波时，必须回到并使用此结构体的变量 */
    /* 规定2：此结构体的变量的初始化需要放在一个函数中 */

    float   CPU_Freq;       /* CPU频率 */
    float   PWM_Freq;       /* PWM频率 */
    float   PWM_CycleTime;  /* PWM周期 */
    float   PWM_Duty;       /* 占空比 */
    float   PWM_DeadTime;   /* ePWM1死区时间 (ns) */
    Uint16  PWM_TBPRD;      /* ePWM1周期寄存器值 */
    Uint16  PWM_AComp;      /* ePWM1A比较值 */
    Uint16  PWM_BComp;      /* ePWM1B比较值 */
};

/***************************** ADC值结构 *****************/
struct _ADC_Values
{
    float   IL;     /* 电感电流 */
    float   Iout;   /* 输出电流 (平均) */
    float   Vout;   /* 输出电压 */
    float   Vin;    /* 输入电压 (平均) */
    float   Iout_Offset;
};

/***************************** 控制器值结构 *****************/
struct  _Ctr_values
{
    /* 规定1：这是对闭环控制参数的汇总结构体，进行的任何计算都需要此机构体的变量 */
    /* 规定2：此结构体的变量仅参与计算，实际控制PWM波时不使用此结构体的变量    */
    /* 规定3：此结构体的变量的初始化需要放在一个函数中 */

    float   I_Error1;        /* 电流误差1 */
    float   I_Error2;        /* 电流误差2 */
    float   I_Out;           /* 电流PI输出 */
    float   I_DeltaOut;      /* 电流PI增量输出 */
    float   V_Error1;        /* 电压误差1 */
    float   V_Error2;        /* 电压误差2 */
    float   V_Out;           /* 电压PI输出 */
    float   V_DeltaOut;      /* 电压PI增量输出 */

    float   V_Ref;           /* 电压参考 */
    float   I_Ref;           /* 电流参考 */
    float   Can_V_Ref;       /* CAN电压参考 */
    float   Can_I_Ref;       /* CAN电流参考 */

    float   Ts;              /* 采样周期 */
    float   V_Kp;            /* 电压Kp */
    float   V_Ki;            /* 电压Ki */
    float   V_Ki_Ts;         /* 电压Ki*Ts */
    float   I_Kp;            /* 电流Kp */
    float   I_Ki;            /* 电流Ki */
    float   I_Ki_Ts;         /* 电流Ki*Ts */

    float   MAX_Duty;        /* 最大占空比 */
    float   MIN_Duty;        /* 最小占空比 */
    float   MAX_DeadTime;    /* 最大死区时间 */
    float   MIN_DeadTime;    /* 最小死区时间 */
    float   MAX_CycleTime;   /* 最大周期时间 */
    float   MIN_CycleTime;   /* 最小周期时间 */
    float   Ctrl_Duty;       /* 控制输出 */
    float   Ctrl_DeadTime;   /* 死区时间 */
    float   Ctrl_CycleTime;  /* PI输出周期时间 (微秒) */

};

/***************************** 函数声明 *****************/
void PI_Init(void);
void PWM_Values_Init(void);
void ValInit(void);
void PWM_Update(void);


void PWM_Disable(void);
void PWM_Enable(void);
void ShortOff(void);
void SlowP(void);
void SwOCP(void);
void SwOVP(void);
void SwUVP(void);
void Process_Valley_Samples(void);
void Process_Peak_Samples(void);
void Adc_Sample_Process(void);

void V_PI_Controller_ComputeIncremental_AntiWindup(void);
void I_PI_Controller_ComputeIncremental_AntiWindup(void);
void State_M(void);
void StateMInit(void);
void StateMWait(void);
void StateMRise(void);
void StateMRun(void);
void StateMErr(void);



#endif /* APP_APP_LIBRARIES_FUNCTION_H_ */
