/**
 * @file function.c
 * @brief Boost变换器控制函数 (旧代码 - 当前未使用)
 * @date 2025
 * @author da
 *
 * 本文件包含Boost变换器实现的控制函数。
 * 注意：这是旧代码。当前项目使用LLC_function.c中的LLC谐振变换器
 * 控制函数。
 */

#include "function.h"

const float ADC_Transform = 3.0f/4095.0f;

struct  _FLAG       Boost_Flag      ={0};
struct  _PWM_Values Boost_PWM       ={0};
struct  _ADC_Values Boost_Values    ={0};
struct  _Ctr_values Boost_Ctr       ={0};

/**
 * @brief 初始化PI控制器参数
 * @param None
 * @return None
 */
void PI_Init(void)
{
    Boost_Ctr.Ts         = 1.0f/160000.0f;
    Boost_Ctr.I_Ki       = 2488.5f;
    Boost_Ctr.I_Ki_Ts    = Boost_Ctr.I_Ki * Boost_Ctr.Ts;
    Boost_Ctr.I_Kp       = 0.0566f;
    Boost_Ctr.V_Ki       = 462.57f;
    Boost_Ctr.V_Ki_Ts    = Boost_Ctr.V_Ki * Boost_Ctr.Ts;
    Boost_Ctr.V_Kp       = 0.0736f;
    Boost_Ctr.V_Ref      = 0.0f;
    Boost_Ctr.I_Ref      = 0.0f;
    Boost_Ctr.Ctrl_Duty         = 0.0f;
    Boost_Ctr.MAX_Duty   = 0.6f;
    Boost_Ctr.MIN_Duty   = 0.1f;
}

/**
 * @brief 初始化PWM时序值
 * @param None
 * @return None
 */
void PWM_Values_Init(void)
{
    Boost_PWM.CPU_Freq = 150000000.0f;
    Boost_PWM.PWM_Freq = 160000.0f;
    Boost_PWM.PWM_Duty = Boost_Ctr.MIN_Duty;
    Boost_PWM.PWM_TBPRD = Boost_PWM.CPU_Freq/(2.0f * Boost_PWM.PWM_Freq);
    Boost_PWM.PWM_AComp = Boost_PWM.PWM_Duty * Boost_PWM.PWM_TBPRD;
    Boost_PWM.PWM_BComp = Boost_PWM.PWM_Duty * Boost_PWM.PWM_TBPRD;
    Boost_PWM.PWM_DeadTime = 15;
    EALLOW;
    EPwm1Regs.DBRED = Boost_PWM.PWM_DeadTime;
    EPwm1Regs.DBFED = Boost_PWM.PWM_DeadTime;
    EDIS;
}

#pragma CODE_SECTION(Adc_Sample_Process, ".TI.ramfunc")
/**
 * @brief 使用IIR滤波器处理ADC采样值
 * @param None
 * @return None
 */
void Adc_Sample_Process(void)
{
    /* ADC采样中间变量 */
    static float Temp_V = 0.0f;
    static float Temp_V_ = 0.0f;
    static float Temp_I = 0.0f;
    static float Temp_I_ = 0.0f;
    static float Temp_Vin = 0.0f;
    static float Temp_Vin_ = 0.0f;
    static float Temp_Iout = 0.0f;
    static float Temp_Iout_ = 0.0f;

    /* 使用IIR滤波器的移动平均 */
    /* 带宽计算: IIR_ALPHA=(2*pi*f_cutoff)/f_sampling */

    Temp_V = (float) ( (AdcRegs.ADCRESULT0)>>4 );
    Temp_V_ = ( Temp_V * ADC_Transform ) * IIR_ALPHA + ( 1 - IIR_ALPHA ) * Temp_V_;
    Boost_Values.Vout = Temp_V_ * 44.4444f;

    Temp_I = (float) ( (AdcRegs.ADCRESULT1)>>4 );
    Temp_I_ = ( Temp_I * ADC_Transform ) * IIR_ALPHA + ( 1 - IIR_ALPHA ) * Temp_I_;
    Boost_Values.IL = Temp_I_ * 44.4444f;

    Temp_Vin = (float) ( (AdcRegs.ADCRESULT2)>>4 );
    Temp_Vin_ = ( Temp_Vin * ADC_Transform ) * IIR_ALPHA + ( 1 - IIR_ALPHA ) * Temp_Vin_;
    Boost_Values.Vin = Temp_Vin_ * 44.4444f;

    Temp_Iout = (float) ( (AdcRegs.ADCRESULT3)>>4 );
    Temp_Iout_ = ( Temp_Iout * ADC_Transform ) * IIR_ALPHA + ( 1 - IIR_ALPHA ) * Temp_Iout_;
    Boost_Values.Iout = Temp_Iout_ * 44.4444f;
}

#pragma CODE_SECTION(Process_Valley_Samples, ".TI.ramfunc")
/**
 * @brief 处理谷值采样 (SEQ1在TBCTR=0时的结果)
 * @param None
 * @return None
 *
 * 读取SEQ1结果，应用IIR滤波器，并更新Boost_Values。
 * PI控制器在此中断中看到最新的谷值。
 */
void Process_Valley_Samples(void)
{
    /* 独立的IIR滤波器状态变量 */
    static float Temp_V_ = 0.0f;
    static float Temp_I_ = 0.0f;
    static float Temp_Vin_ = 0.0f;
    static float Temp_Iout_ = 0.0f;

    /* 读取SEQ1结果 */
    float Temp_V = (float)(AdcRegs.ADCRESULT0 >> 4);
    float Temp_I = (float)(AdcRegs.ADCRESULT1 >> 4);
    float Temp_Vin = (float)(AdcRegs.ADCRESULT2 >> 4);
    float Temp_Iout = (float)(AdcRegs.ADCRESULT3 >> 4);

    /* 滤波、缩放并更新全局变量 */
    Temp_V_ = (Temp_V * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * Temp_V_;
    Boost_Values.Vout = Temp_V_ * 44.4444f;

    Temp_I_ = (Temp_I * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * Temp_I_;
    Boost_Values.IL = Temp_I_ * 44.4444f; /* PI控制器使用滤波后的谷值电流 */

    Temp_Vin_ = (Temp_Vin * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * Temp_Vin_;
    Boost_Values.Vin = Temp_Vin_ * 44.4444f;

    Temp_Iout_ = (Temp_Iout * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * Temp_Iout_;
    Boost_Values.Iout = Temp_Iout_ * 44.4444f;
}

#pragma CODE_SECTION(Process_Peak_Samples, ".TI.ramfunc")
/**
 * @brief 处理峰值采样 (SEQ2在TBCTR=TBPRD时的结果)
 * @param None
 * @return None
 *
 * 读取SEQ2结果，应用IIR滤波器，并更新Boost_Values。
 * PI控制器在此中断中看到最新的峰值。
 */
void Process_Peak_Samples(void)
{
    /* 独立的IIR滤波器状态变量 */
    static float Temp_V_peak_ = 0.0f;
    static float Temp_I_peak_ = 0.0f;
    static float Temp_Vin_peak_ = 0.0f;
    static float Temp_Iout_peak_ = 0.0f;

    /* 读取SEQ2结果 */
    float Temp_V_peak = (float)(AdcRegs.ADCRESULT4 >> 4);
    float Temp_I_peak = (float)(AdcRegs.ADCRESULT5 >> 4);
    float Temp_Vin_peak = (float)(AdcRegs.ADCRESULT6 >> 4);
    float Temp_Iout_peak = (float)(AdcRegs.ADCRESULT7 >> 4);

    /* 滤波、缩放并更新全局变量 */
    Temp_V_peak_ = (Temp_V_peak * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * Temp_V_peak_;
    Boost_Values.Vout = Temp_V_peak_ * 44.4444f;

    Temp_I_peak_ = (Temp_I_peak * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * Temp_I_peak_;
    Boost_Values.IL = Temp_I_peak_ * 44.4444f; /* PI控制器使用滤波后的峰值电流 */

    Temp_Vin_peak_ = (Temp_Vin_peak * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * Temp_Vin_peak_;
    Boost_Values.Vin = Temp_Vin_peak_ * 44.4444f;

    Temp_Iout_peak_ = (Temp_Iout_peak * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * Temp_Iout_peak_;
    Boost_Values.Iout = Temp_Iout_peak_ * 44.4444f;
}

#pragma CODE_SECTION(PWM_Update, ".TI.ramfunc")
/**
 * @brief 更新PWM比较值
 * @param None
 * @return None
 */
void PWM_Update(void)
{
    static Uint16  CompareValue = 0;

    Boost_PWM.PWM_Duty = Boost_Ctr.Ctrl_Duty;

    CompareValue = (Uint16) ( Boost_PWM.PWM_TBPRD-Boost_PWM.PWM_TBPRD*Boost_PWM.PWM_Duty );

    EALLOW;
    EPwm1Regs.CMPA.half.CMPA = CompareValue;  /* 设置占空比 */
    EDIS;
}

#pragma CODE_SECTION(PWM_Disable, ".TI.ramfunc")
/**
 * @brief 通过触发区禁用PWM输出
 * @param None
 * @return None
 */
void PWM_Disable(void)
{
    EALLOW;
    /* 通过软件强制产生一次性触发事件 */
    /* 立即激活TZCTL动作 (例如, TZ_FORCE_LO) */
    EPwm1Regs.TZFRC.bit.OST = 1;
    EDIS;
}

#pragma CODE_SECTION(PWM_Enable, ".TI.ramfunc")
/**
 * @brief 通过清除触发区使能PWM输出
 * @param None
 * @return None
 */
void PWM_Enable(void)
{
    EALLOW;

    /* 必须清除触发标志以恢复PWM */
    /* 清除OST标志，因为我们强制了OST */
    /* 向TZCLR位写1以清除标志 */
    EPwm1Regs.TZCLR.bit.OST = 1;

    /* 可选：在重新使能时清除PI状态和PWM计数器 */
    /* 确保干净的软启动 */

    EDIS;
}

#pragma CODE_SECTION(ShortOff, ".TI.ramfunc")
#define I_OUT_SHORT_MAX 15.0f
#define V_OUT_SHORT_MIN 7.5f
/**
 * @brief 短路保护
 * @param None
 * @return None
 *
 * 当Iout>15A且Vout<7.5V时检测到短路
 */
void ShortOff(void)
{
    /* 短路: Iout>15A 且 Vout<7.5V */
    if((Boost_Values.Iout> I_OUT_SHORT_MAX)&&(Boost_Values.Vout <V_OUT_SHORT_MIN))
    {
        /* 禁用PWM */
        PWM_Disable();
        /* 设置故障标志 */
        Boost_Flag.Error = Error_OUT_SHORT;
        /* 跳转到错误状态 */
        Boost_Flag.StateM  = Err;
    }
}

#pragma CODE_SECTION(State_M, ".TI.ramfunc")
/**
 * @brief 状态机主处理函数
 * @param None
 * @return None
 */
void State_M(void)
{
    switch(Boost_Flag.StateM)
    {
        /* 初始化状态 */
        case  Init :StateMInit();
        break;
        /* 等待状态 */
        case  Wait :StateMWait();
        break;
        /* 软启动状态 */
        case  Rise :StateMRise();
        break;
        /* 运行状态 */
        case  Run :StateMRun();
        break;
        /* 错误状态 */
        case  Err :StateMErr();
        break;
    }
}

#pragma CODE_SECTION(StateMInit, ".TI.ramfunc")
/**
 * @brief 初始化状态处理函数
 * @param None
 * @return None
 */
void StateMInit(void)
{
    /* 初始化参数 */
    ValInit();
    /* 状态机 -> 等待软启动 */
    LED2_OFF;
    LED3_OFF;
    LED4_OFF;
    LED5_OFF;
    LED1_ON;
    Boost_Flag.StateM  = Wait;
}

#pragma CODE_SECTION(ValInit, ".TI.ramfunc")
/**
 * @brief 初始化值
 * @param None
 * @return None
 */
void ValInit(void)
{
    /* 清除故障标志 */
    Boost_Flag.Error = 0;
}

#pragma CODE_SECTION(StateMWait, ".TI.ramfunc")
/**
 * @brief 等待状态处理函数
 * @param None
 * @return None
 */
void StateMWait(void)
{
    LED1_OFF;
    LED3_OFF;
    LED4_OFF;
    LED5_OFF;
    LED2_ON;
    /* 计数器定义 */
     static unsigned int CntS = 0;

     /* 递增计数器 */
    CntS ++;
    /* 等待1.5秒，如果无故障则进入启动状态 */
    if((CntS>300)&& (Boost_Flag.Error==Error_NOERR))
     {
        /* 清除计数器 */
        CntS=0;
        /* 状态标志 -> 等待状态 */
        Boost_Flag.StateM = Rise;
     }
}

#pragma CODE_SECTION(StateMRise, ".TI.ramfunc")
#define V_TARGET        75.0f   /* 目标输出电压 */
#define V_REF_START     0.0f    /* 软启动初始电压 */
#define V_REF_STEP      0.2f    /* 每次调用的电压步进 */
#define I_TARGET        75.0f   /* 目标输出电流 */
#define I_REF_START     0.0f    /* 软启动初始电流 */
#define I_REF_STEP      0.05f   /* 每次调用的电流步进 */
/**
 * @brief 软启动（上升）状态处理函数
 * @param None
 * @return None
 */
void StateMRise(void)
{
    /* 静态标志确保初始化代码在第一次进入时只运行一次 */
    static unsigned char is_first_entry = 1;

    LED1_OFF;
    LED2_OFF;
    LED4_OFF;
    LED5_OFF;
    LED3_ON; /* 软启动状态指示灯 */

    /* --- 1. 初始化 (首次进入时运行一次) --- */
    if (is_first_entry)
    {
        /* a. 复位标志 */
        is_first_entry = 0;

        /* b. 初始化软启动参数 */
        /* 关键: 从0V或一个小的安全电压开始 */
        Boost_Ctr.V_Ref = V_REF_START;
        Boost_Ctr.I_Ref = I_REF_START;
        Boost_Ctr.Ctrl_Duty    = Boost_Ctr.MIN_Duty;   /* 确保占空比从最小值开始 */

        /* c. 初始化PI控制器状态 (关键!) */
        /* 清除历史记录以避免残留的积分饱和 */
        Boost_Ctr.V_Error1      = 0.0f;
        Boost_Ctr.V_Error2      = 0.0f;
        Boost_Ctr.V_Out         = 0.0f;
        Boost_Ctr.V_DeltaOut    = 0.0f;
        Boost_Ctr.I_Error1      = 0.0f;
        Boost_Ctr.I_Error2      = 0.0f;
        Boost_Ctr.I_Out         = 0.0f;
        Boost_Ctr.I_DeltaOut    = 0.0f;

        /* d. 确保PWM准备就绪并已使能 */
        PWM_Update();       /* 应用初始的0占空比 */
        PWM_Enable();       /* 使能PWM输出 */
    }

    /* --- 2. 执行 (每次调用都运行) --- */
    /* 检查是否达到目标电压 */
    if (Boost_Ctr.V_Ref < V_TARGET)
    {
        /* a. 斜坡上升参考电压 */
        Boost_Ctr.V_Ref += V_REF_STEP;
    }
    else
    {
        /* b. 软启动完成 */
        /* 钳位到最终目标值以防止过冲 */
        Boost_Ctr.V_Ref     = V_TARGET;
        Boost_Flag.StateM   = Run;          /* 切换到正常运行状态 */
        /* 关键: 为下一次软启动复位标志 */
        is_first_entry      = 1;
    }
}

#pragma CODE_SECTION(StateMRun, ".TI.ramfunc")
/**
 * @brief 运行状态处理函数
 * @param None
 * @return None
 */
void StateMRun(void)
{
    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
    LED5_OFF;
    LED4_ON;
}

#pragma CODE_SECTION(StateMErr, ".TI.ramfunc")
unsigned char   OppFlag=0;
/**
 * @brief 错误状态处理函数
 * @param None
 * @return None
 */
void StateMErr(void)
{
    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
    LED4_OFF;
    LED5_ON;

    /* 禁用PWM */
    PWM_Disable();
}

#pragma CODE_SECTION(SlowP, ".TI.ramfunc")
/**
 * @brief 慢速保护处理函数
 * @param None
 * @return None
 */
void SlowP(void)
{
    /* 软件过流保护 */
    SwOCP();
    /* 软件过压保护 */
    SwOVP();
    /* 欠压保护 */
    SwUVP();
}

#pragma CODE_SECTION(SwOCP, ".TI.ramfunc")
#define MAX_OCP_VAL     15.0f   /* 15A 过流保护阈值 */
/**
 * @brief 软件过流保护
 * @param None
 * @return None
 */
void SwOCP(void)
{
    /* OCP计数器定义 */
     static  unsigned int  OCPCnt=0;

    /* 如果在运行状态下Iout>15A持续500ms */
    if((Boost_Values.IL > MAX_OCP_VAL)&&(Boost_Flag.StateM  ==Run))
    {
        /* 条件保持计时器 */
        OCPCnt++;
        /* 保持500ms -> OCP故障 */
        if(OCPCnt > 100)
        {
            /* 清除计数器 */
            OCPCnt  = 0;
            /* 禁用PWM */
            PWM_Disable();
            /* 设置故障标志 */
            setRegBits(Boost_Flag.Error,Error_OCP);
            /* 跳转到错误状态 */
            Boost_Flag.StateM  =Err;
        }
    }
    else
        /* 清除计数器 */
        OCPCnt  = 0;
}

#pragma CODE_SECTION(SwOVP, ".TI.ramfunc")
#define MAX_OVP_VAL    150.0f   /* 150V 过压保护 */
/**
 * @brief 软件过压保护
 * @param None
 * @return None
 */
void SwOVP(void)
{
    /* OVP计数器定义 */
    static  unsigned int  OVPCnt=0;

    /* 如果Vout>150V持续100ms */
    if (Boost_Values.Vout > MAX_OVP_VAL)
    {
        /* 条件保持计时器 */
        OVPCnt++;
        /* 保持100ms */
        if(OVPCnt > 20)
        {
            /* 清除计时器 */
            OVPCnt=0;
            /* 禁用PWM */
            PWM_Disable();
            /* 设置故障标志 */
            setRegBits(Boost_Flag.Error,Error_OVP);
            /* 跳转到错误状态 */
            Boost_Flag.StateM = Err;
        }
    }
    else
        OVPCnt = 0;
}

#pragma CODE_SECTION(SwUVP, ".TI.ramfunc")
#define MIN_UVP_VAL     25.0f   /* 25V 欠压保护 */
/**
 * @brief 软件欠压保护
 * @param None
 * @return None
 */
void SwUVP(void)
{
    /* UVP计数器定义 */
    static  unsigned int  UVPCnt=0;

    /* 如果在运行状态下Vout<25V持续500ms */
    if ((Boost_Values.Vout < MIN_UVP_VAL)&&(Boost_Flag.StateM == Run))
    {
        /* 条件保持计时器 */
        UVPCnt++;
        /* 保持500ms */
        if(UVPCnt> 100)
        {
            /* 清除计时器 */
            UVPCnt=0;
            /* 禁用PWM */
            PWM_Disable();
            /* 设置故障标志 */
            setRegBits(Boost_Flag.Error,Error_UVP);
            /* 跳转到错误状态 */
            Boost_Flag.StateM  =Err;
        }
    }
    else
        UVPCnt = 0;
}

#pragma CODE_SECTION(V_PI_Controller_ComputeIncremental_AntiWindup, ".TI.ramfunc")
#define V_Error_MAX 5.0f
#define V_Error_MIN -5.0f
#define V_PIOUT_MAX 9.0f
#define V_PIOUT_MIN 0.0f

/**
 * @brief 带抗饱和的电压PI控制器
 * @param None
 * @return None
 */
void V_PI_Controller_ComputeIncremental_AntiWindup(void)
{
    float pi_out_temp; /* 用于存储未钳位输出的临时变量 */

    /* 1. 计算电压误差 */
    Boost_Ctr.V_Error1 = Boost_Ctr.V_Ref - Boost_Values.Vout;

    /* 2. 钳制电压误差 */
    if(Boost_Ctr.V_Error1 > V_Error_MAX) {
        Boost_Ctr.V_Error1 = V_Error_MAX;
    }
    else if(Boost_Ctr.V_Error1 < V_Error_MIN) {
        Boost_Ctr.V_Error1 = V_Error_MIN;
    }

    /* 3. 计算 DeltaOut (增量) */
    Boost_Ctr.V_DeltaOut = Boost_Ctr.V_Kp * (Boost_Ctr.V_Error1 - Boost_Ctr.V_Error2) + Boost_Ctr.V_Ki_Ts * Boost_Ctr.V_Error1;

    /* 4. 计算临时PI输出 */
    pi_out_temp = Boost_Ctr.V_Out + Boost_Ctr.V_DeltaOut;

    /* 5. 钳制输出并赋给I_Ref */
    if(pi_out_temp > V_PIOUT_MAX) {
        Boost_Ctr.I_Ref = V_PIOUT_MAX;
    }
    else if(pi_out_temp < V_PIOUT_MIN) {
        Boost_Ctr.I_Ref = V_PIOUT_MIN;
    }
    else {
        Boost_Ctr.I_Ref = pi_out_temp;
    }

    /* 6. 抗饱和: 仅在未饱和时更新积分项 */
    if (pi_out_temp > V_PIOUT_MAX || pi_out_temp < V_PIOUT_MIN)
    {
        Boost_Ctr.V_Out = Boost_Ctr.I_Ref; /* 饱和 - 冻结 */
    }
    else
    {
        Boost_Ctr.V_Out = pi_out_temp; /* 未饱和 */
    }

    /* 7. 更新历史误差 */
    Boost_Ctr.V_Error2 = Boost_Ctr.V_Error1;
}

#pragma CODE_SECTION(I_PI_Controller_ComputeIncremental_AntiWindup, ".TI.ramfunc")
/**
 * @brief 带抗饱和的电流PI控制器
 * @param None
 * @return None
 */
void I_PI_Controller_ComputeIncremental_AntiWindup(void)
{
    float pi_out_temp; /* 用于存储未钳位输出的临时变量 */

    /* 1. 计算电流误差 */
    Boost_Ctr.I_Error1 = Boost_Ctr.I_Ref - Boost_Values.IL;

    /* 2. 计算 DeltaOut (增量) */
    Boost_Ctr.I_DeltaOut = Boost_Ctr.I_Kp * (Boost_Ctr.I_Error1 - Boost_Ctr.I_Error2) + Boost_Ctr.I_Ki_Ts * Boost_Ctr.I_Error1;

    /* 3. 计算临时PI输出 */
    pi_out_temp = Boost_Ctr.I_Out + Boost_Ctr.I_DeltaOut;

    /* 4. 钳制输出并赋给Vc */
    if(pi_out_temp > Boost_Ctr.MAX_Duty) {
        Boost_Ctr.Ctrl_Duty = Boost_Ctr.MAX_Duty;
    }
    else if(pi_out_temp < Boost_Ctr.MIN_Duty) {
        Boost_Ctr.Ctrl_Duty = Boost_Ctr.MIN_Duty;
    }
    else {
        Boost_Ctr.Ctrl_Duty = pi_out_temp;
    }

    /* 5. 抗饱和核心: 仅在未饱和时更新积分项 */
    if (pi_out_temp > Boost_Ctr.MAX_Duty || pi_out_temp < Boost_Ctr.MIN_Duty)
    {
        Boost_Ctr.I_Out = Boost_Ctr.Ctrl_Duty; /* 钳位 - 冻结积分 */
    }
    else
    {
        Boost_Ctr.I_Out = pi_out_temp; /* 未饱和 - 更新积分 */
    }

    /* 6. 更新历史误差 */
    Boost_Ctr.I_Error2 = Boost_Ctr.I_Error1;
}


