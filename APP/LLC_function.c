/**
 * @file LLC_function.c
 * @brief LLC谐振变换器控制函数
 * @date 2025
 * @author da
 *
 * 本文件实现了LLC谐振变换器的控制算法，
 * 包括电压PI控制、频率调制和软启动状态机。
 */

#include "LLC_function.h"
#include "function.h"

extern const float ADC_Transform;

struct  _FLAG       LLC_Flag      ={0};
struct  _PWM_Values LLC_PWM       ={0};
struct  _ADC_Values LLC_Values    ={0};
struct  _Ctr_values LLC_Ctr       ={0};

S_State_M Soft_State_M = SSInit;
volatile unsigned int g_Soft_State_M_for_Graph = 0;

unsigned  char RsingInitFlag = 0;

/**
 * @brief 初始化LLC PI控制器参数
 * @param None
 * @return None
 */
void LLC_Ctr_Init(void)
{
    /* 1. 误差与PI中间变量初始化 */
    LLC_Ctr.I_Error1   = 0.0f;
    LLC_Ctr.I_Error2   = 0.0f;
    LLC_Ctr.I_Out      = 0.0f;
    LLC_Ctr.I_DeltaOut = 0.0f;

    LLC_Ctr.V_Error1   = 0.0f;
    LLC_Ctr.V_Error2   = 0.0f;
    LLC_Ctr.V_Out      = 0.0f;
    LLC_Ctr.V_DeltaOut = 0.0f;

    /* 2. 参考值初始化 */
    LLC_Ctr.V_Ref      = 400.0f;
    LLC_Ctr.I_Ref      = 0.0f;
    LLC_Ctr.Can_V_Ref  = 0.0f;
    LLC_Ctr.Can_I_Ref  = 0.0f;

    /* 3. 控制周期与PI参数初始化 */
    LLC_Ctr.Ts         = 1.0f / fs;

    LLC_Ctr.I_Kp       = 0.0f;
    LLC_Ctr.I_Ki       = 0.0f;
    LLC_Ctr.I_Ki_Ts    = LLC_Ctr.I_Ki * LLC_Ctr.Ts;

    LLC_Ctr.V_Kp       = 150.0f;
    LLC_Ctr.V_Ki       = 150000.0f;
    LLC_Ctr.V_Ki_Ts    = LLC_Ctr.V_Ki * LLC_Ctr.Ts;

    /* 4. 控制量上下限初始化 */
    LLC_Ctr.MAX_Duty      = 0.6f;
    LLC_Ctr.MIN_Duty      = 0.1f;

    LLC_Ctr.MAX_DeadTime  = 300.0f;
    LLC_Ctr.MIN_DeadTime  = 50.0f;

    /* 周期时间控制：300kHz -> 3.33us, 1000kHz -> 1.0us */
    LLC_Ctr.MAX_CycleTime = 3.33f;
    LLC_Ctr.MIN_CycleTime = 1.0f;

    /* 5. 实际控制输出初始化 */
    LLC_Ctr.Ctrl_Duty      = 0.5f;
    LLC_Ctr.Ctrl_DeadTime  = LLC_Ctr.MAX_DeadTime;
    LLC_Ctr.Ctrl_CycleTime = LLC_Ctr.MIN_CycleTime;
}

/**
 * @brief 初始化LLC PWM时序值
 * @param None
 * @return None
 */
void LLC_PWM_Values_Init(void)
{
    /* 基本PWM参数初始化 */
    LLC_PWM.CPU_Freq      = 150000000.0f;          /* Hz */
    LLC_PWM.PWM_Freq      = 400000.0f;             /* Hz */
    LLC_PWM.PWM_CycleTime = 1.0f / LLC_PWM.PWM_Freq; /* s */

    /* 初始占空比 */
    LLC_PWM.PWM_Duty      = LLC_Ctr.MIN_Duty;

    /* up-down模式下: TBPRD = TBCLK / (2 * fPWM)
       当前默认 TBCLK = CPU_Freq (CLKDIV=1, HSPCLKDIV=1) */
    LLC_PWM.PWM_TBPRD     = LLC_PWM.CPU_Freq * 0.5f * LLC_PWM.PWM_CycleTime;

    /* 比较值 */
    LLC_PWM.PWM_AComp     = LLC_PWM.PWM_Duty * LLC_PWM.PWM_TBPRD;
    LLC_PWM.PWM_BComp     = LLC_PWM.PWM_Duty * LLC_PWM.PWM_TBPRD;

    /* 死区时间，单位 ns */
    LLC_PWM.PWM_DeadTime  = 20.0f;

    /* 将死区时间从 ns 转为 DBRED/DBFED 计数
       count = t_dead(ns) * TBCLK(Hz) / 1e9 */
    EALLOW;
    EPwm1Regs.DBRED = (Uint16)(LLC_PWM.PWM_DeadTime * 0.15f + 0.5f);
    EPwm1Regs.DBFED = (Uint16)(LLC_PWM.PWM_DeadTime * 0.15f + 0.5f);
    EDIS;
}

#pragma CODE_SECTION(LLC_ValInit, ".TI.ramfunc")
/**
 * @brief 初始化LLC控制值
 * @param None
 * @return None
 */
void LLC_ValInit(void)
{
    /* 清除错误标志 */
    LLC_Flag.Error = 0;

    LLC_Ctr.I_Ref       = 0.0f;
    LLC_Ctr.V_Ref       = 0.0f;
    LLC_Ctr.Can_V_Ref   = 400.0f;
    LLC_Ctr.Can_I_Ref   = 1.8f;
    /* 初始化周期时间为谐振频率点 (2.5us @ 400kHz) */
    LLC_Ctr.Ctrl_CycleTime = LLC_RESONANT_CYCLE_US;
}

#pragma CODE_SECTION(LLC_PWM_Update, ".TI.ramfunc")
/**
 * @brief 根据PI输出更新LLC PWM频率
 * @param None
 * @return None
 */
void LLC_PWM_Update(void)
{
    const float TBPRD_SCALE = 75.0f;    /* 150MHz, up-down, div=1 */
                                        /* TBPRD = 150e6 / 2 * CycleTime_us * 1e-6 = 75 * CycleTime_us */

    LLC_PWM.PWM_TBPRD = (Uint16)(TBPRD_SCALE * LLC_Ctr.Ctrl_CycleTime + 0.5f);
    if (LLC_PWM.PWM_TBPRD < 2)
        LLC_PWM.PWM_TBPRD = 2;

    LLC_PWM.PWM_AComp = LLC_PWM.PWM_TBPRD >> 1;   /* 50% duty */
    if (LLC_PWM.PWM_AComp < 1)
        LLC_PWM.PWM_AComp = 1;
    else if (LLC_PWM.PWM_AComp >= LLC_PWM.PWM_TBPRD)
        LLC_PWM.PWM_AComp = LLC_PWM.PWM_TBPRD - 1;

    LLC_PWM.PWM_CycleTime = LLC_Ctr.Ctrl_CycleTime;          /* 单位自己统一 */
    LLC_PWM.PWM_DeadTime = LLC_Ctr.Ctrl_DeadTime;

    if (LLC_PWM.PWM_DeadTime < 0.0f)
        LLC_PWM.PWM_DeadTime = 0.0f;

    EALLOW;
    EPwm1Regs.TBPRD = LLC_PWM.PWM_TBPRD;
    EPwm1Regs.CMPA.half.CMPA = LLC_PWM.PWM_AComp;
    EPwm1Regs.DBRED = (Uint16)(LLC_PWM.PWM_DeadTime * 0.15f + 0.5f);
    EPwm1Regs.DBFED = (Uint16)(LLC_PWM.PWM_DeadTime * 0.15f + 0.5f);
    EDIS;
}



#pragma CODE_SECTION(LLC_V_PI_Controller_ComputeIncremental_AntiWindup, ".TI.ramfunc")
#define LLC_V_Error_MAX 20.0f
#define LLC_V_Error_MIN -20.0f
/* PI输出限幅更改为周期时间增量 (单位: 微秒 us) */
/* 工作点: 400kHz -> 2.5us 中心 */
#define LLC_V_PIOUT_MAX_CYCLE  1.5f    /* 最大可减小至 1.0us (1MHz) */
#define LLC_V_PIOUT_MIN_CYCLE -0.83f   /* 最大可增加至 3.33us (300kHz) */

/**
 * @brief 带抗饱和的LLC电压PI控制器
 * @param None
 * @return None
 */
void LLC_V_PI_Controller_ComputeIncremental_AntiWindup(void)
{
    float u_unsat;
    float u_sat;
    const float CYCLE_CENTER = LLC_RESONANT_CYCLE_US;

    /* 1. 误差 */
    LLC_Ctr.V_Error1 = LLC_Ctr.V_Ref - LLC_Values.Vout;

    /* 2. 误差限幅 */
    if (LLC_Ctr.V_Error1 > LLC_V_Error_MAX)
        LLC_Ctr.V_Error1 = LLC_V_Error_MAX;
    else if (LLC_Ctr.V_Error1 < LLC_V_Error_MIN)
        LLC_Ctr.V_Error1 = LLC_V_Error_MIN;

    /* 3. 增量PI */
    LLC_Ctr.V_DeltaOut = LLC_Ctr.V_Kp * (LLC_Ctr.V_Error1 - LLC_Ctr.V_Error2)
                       + LLC_Ctr.V_Ki_Ts * LLC_Ctr.V_Error1;

    /* 4. 未限幅输出 */
    u_unsat = LLC_Ctr.V_Out + LLC_Ctr.V_DeltaOut;

    /* 5. 输出限幅 */
    u_sat = u_unsat;
    if (u_sat > LLC_V_PIOUT_MAX_CYCLE)
        u_sat = LLC_V_PIOUT_MAX_CYCLE;
    else if (u_sat < LLC_V_PIOUT_MIN_CYCLE)
        u_sat = LLC_V_PIOUT_MIN_CYCLE;

    /* 6. 内部状态更新：钳位到边界 */
    LLC_Ctr.V_Out = u_sat;

    /* 7. 更新误差历史 */
    LLC_Ctr.V_Error2 = LLC_Ctr.V_Error1;

    /* 8. 输出到周期 */
    LLC_Ctr.Ctrl_CycleTime = CYCLE_CENTER - u_sat;   // 这里重点检查符号方向

    /* 9. 最终安全限幅 */
    if (LLC_Ctr.Ctrl_CycleTime > LLC_Ctr.MAX_CycleTime)
        LLC_Ctr.Ctrl_CycleTime = LLC_Ctr.MAX_CycleTime;
    else if (LLC_Ctr.Ctrl_CycleTime < LLC_Ctr.MIN_CycleTime)
        LLC_Ctr.Ctrl_CycleTime = LLC_Ctr.MIN_CycleTime;
}

#pragma CODE_SECTION(LLC_I_PI_Controller_ComputeIncremental_AntiWindup, ".TI.ramfunc")
/**
 * @brief 电流PI控制器占位符 (未使用)
 * @param None
 * @return None
 */
void LLC_I_PI_Controller_ComputeIncremental_AntiWindup(void)
{
    /* 当前在LLC中未实现 */
}

#pragma CODE_SECTION(LLC_State_M, ".TI.ramfunc")
/**
 * @brief LLC状态机主处理函数
 * @param None
 * @return None
 */
void LLC_State_M(void)
{
    switch(LLC_Flag.StateM)
    {
        case  Init :LLC_StateMInit();   break;  /* 初始化状态 */
        case  Wait :LLC_StateMWait();   break;  /* 等待状态 */
        case  Rise :LLC_StateMRise();   break;  /* 软启动状态 */
        case  Run :LLC_StateMRun();     break;  /* 运行状态 */
        case  Err :LLC_StateMErr();     break;  /* 错误状态 */
    }
}

#pragma CODE_SECTION(LLC_StateMInit, ".TI.ramfunc")
/**
 * @brief 初始化状态处理函数
 * @param None
 * @return None
 */
void LLC_StateMInit(void)
{
    static char StateM_FirstFlag = 0;
    if(StateM_FirstFlag==0)
    {
        StateM_FirstFlag=1;
        LLC_ValInit();
    }
    if(IoutOffsetCal()==1)
    {
        LLC_Flag.StateM  = Wait;
    }
}

#pragma CODE_SECTION(LLC_StateMWait, ".TI.ramfunc")
/**
 * @brief 等待状态处理函数
 * @param None
 * @return None
 */
void LLC_StateMWait(void)
{
    static unsigned int LLC_CntS = 0;

    if (LLC_Flag.Error != Error_NOERR)
    {
        LLC_CntS = 0;
        return;
    }

    LLC_CntS++;

    if (LLC_CntS >= 600)   /* 600 * 2.5ms = 1.5s */
    {
        LLC_CntS = 0;
        LLC_Flag.StateM = Rise;
        return;
    }
}

#pragma CODE_SECTION(LLC_StateMRise, ".TI.ramfunc")
#define LLC_MAX_Rise_Cnt         200        /* 等待1000ms */
#define LLC_CycleTime_Transfomer 1000000.0f /* 周期时间转换常数 */
#define LLC_MAX_CycleTime 3.33f             /* 最大周期 = 最小频率 300kHz */
#define LLC_MIN_CycleTime 1.0f              /* 最小周期 = 最大频率 1000kHz */
#define LLC_MAX_DeadTime  300.0f
#define LLC_MIN_DeadTime  50.0f

/**
 * @brief 带子状态机的软启动状态处理函数
 * @param None
 * @return None
 */
void LLC_StateMRise(void)
{
    static  unsigned int  LLC_Rise_Cnt = 0;

    switch(Soft_State_M)
    {
        case SSInit:    /* 软启动初始状态 */
        {
            RsingInitFlag = 0;
            PWM_Disable();
            LLC_Ctr.MAX_CycleTime = LLC_MIN_CycleTime;  /* 软启动时使用最高频率 */
            LLC_Ctr.Ctrl_Duty = 0.5f;
            LLC_Ctr.Ctrl_DeadTime = LLC_MAX_DeadTime;
            LLC_Ctr.V_Ref = 100.0f;
            Soft_State_M = SSWait;
            g_Soft_State_M_for_Graph = Soft_State_M;
            break;
        }
        case SSWait:    /* 等待软启动 */
        {
            LLC_Rise_Cnt++;
            if(LLC_Rise_Cnt > LLC_MAX_Rise_Cnt)
            {
                LLC_Rise_Cnt = 0;
                PWM_Enable();
                Soft_State_M = SSRun;
                g_Soft_State_M_for_Graph = Soft_State_M;
            }
            break;
        }
        case SSRun:    /* 软启动运行状态 */
        {
            RsingInitFlag = 1;

            /* 软启动期间逐渐增加周期时间(降低频率) */
            LLC_Ctr.MAX_CycleTime += 0.0029125f;
            LLC_Ctr.Ctrl_CycleTime = LLC_Ctr.MAX_CycleTime;

            /* 软启动期间逐渐减小死区时间 */
            LLC_Ctr.Ctrl_DeadTime -= 0.275f;

            /* 限制最大周期时间 */
            if (LLC_Ctr.MAX_CycleTime > LLC_MAX_CycleTime)
            {
                LLC_Ctr.MAX_CycleTime = LLC_MAX_CycleTime;
            }

            /* 限制最小死区时间 */
            if (LLC_Ctr.Ctrl_DeadTime < LLC_MIN_DeadTime)
            {
                LLC_Ctr.Ctrl_DeadTime = LLC_MIN_DeadTime;
            }

            /* 当周期和死区都达到目标时，软启动完成 */
            if ((LLC_Ctr.MAX_CycleTime >= LLC_MAX_CycleTime) &&
                (LLC_Ctr.Ctrl_DeadTime <= LLC_MIN_DeadTime))
            {
                LLC_Flag.StateM = Run;
                Soft_State_M = SSInit;
                g_Soft_State_M_for_Graph = Soft_State_M;
            }
            break;
        }
    }
}

#pragma CODE_SECTION(LLC_StateMRun, ".TI.ramfunc")
/**
 * @brief 运行状态处理函数
 * @param None
 * @return None
 */
void LLC_StateMRun(void)
{
    /* 正常运行 - 此处可添加电压/电流保护逻辑 */
}

#pragma CODE_SECTION(LLC_StateMErr, ".TI.ramfunc")
/**
 * @brief 错误状态处理函数
 * @param None
 * @return None
 */
void LLC_StateMErr(void)
{
    PWM_Disable();
}

#pragma CODE_SECTION(LLC_V_PI_Clear, ".TI.ramfunc")
/**
 * @brief 清除电压PI控制器状态
 * @param None
 * @return None
 */
void LLC_V_PI_Clear(void)
{
    LLC_Ctr.V_DeltaOut  = 0.0f;
    LLC_Ctr.V_Error1    = 0.0f;
    LLC_Ctr.V_Error2    = 0.0f;
    LLC_Ctr.V_Out       = 0.0f;
    /* 将周期时间重置到谐振点 */
    LLC_Ctr.Ctrl_CycleTime = LLC_RESONANT_CYCLE_US;
}

/**
 * @brief Burst模式控制
 * @param None
 * @return None
 *
 * 用于轻载效率的Burst模式。
 * 当周期时间低于阈值时，关闭PWM以节省功耗。
 * 当负载增加时，重新使能PWM。
 */
#define     BURST_ON_CYCLE_THRESHOLD    1.4f   /* 进入Burst模式的周期阈值 (us) */
#define     BURST_OFF_CYCLE_THRESHOLD   1.6f   /* 退出Burst模式的周期阈值 (us) */

void LLC_BurstCtl(void)
{
    static unsigned char BurstOffFlag = 0;
    static unsigned int BurstOnCnt = 0;
    static unsigned int BurstOffCnt = 0;

    /* Burst模式仅在运行状态下允许 */
    if(LLC_Flag.StateM != Run)
    {
        BurstOnCnt = 0;
        BurstOffCnt = 0;
        BurstOffFlag = 0;
        return;
    }

    /* Burst模式被禁用 */
    if(!getRegBits(LLC_Flag.Ctr, F_BURST))
    {
        BurstOnCnt = 0;
        BurstOffCnt = 0;

        if(BurstOffFlag)
        {
            BurstOffFlag = 0;
            PWM_Enable();
        }
        return;
    }

    /* 进入Burst: 轻载 -> 更高频率 -> 更小周期时间 */
    if(LLC_Ctr.Ctrl_CycleTime < BURST_ON_CYCLE_THRESHOLD)
    {
        BurstOffCnt = 0;

        if(BurstOnCnt < 8)
            BurstOnCnt++;

        if((BurstOnCnt >= 8) && (!BurstOffFlag))
        {
            BurstOffFlag = 1;
            PWM_Disable();
        }
    }
    /* 退出Burst */
    else if(LLC_Ctr.Ctrl_CycleTime > BURST_OFF_CYCLE_THRESHOLD)
    {
        BurstOnCnt = 0;

        if(BurstOffCnt < 8)
            BurstOffCnt++;

        if((BurstOffCnt >= 8) && BurstOffFlag)
        {
            BurstOffFlag = 0;
            PWM_Enable();
        }
    }
    else
    {
        /* 在迟滞窗口内, 保持当前状态 */
        BurstOnCnt = 0;
        BurstOffCnt = 0;
    }
}

#pragma CODE_SECTION(LLC_ADC_ReadPrevious_And_StartNext, ".TI.ramfunc")
volatile Uint16 g_llc_adc_started = 0;
volatile Uint16 g_llc_adc_valid   = 0;

void LLC_ADC_ReadPrevious_And_StartNext(void)
{
    static float LLC_Temp_V_    = 0.0f;
    static float LLC_Temp_I_    = 0.0f;
    static float LLC_Temp_Vin_  = 0.0f;
    static float LLC_Temp_Iout_ = 0.0f;

    /* 1. 先读取“上一次已经完成”的结果 */
    if (g_llc_adc_started && (AdcRegs.ADCST.bit.INT_SEQ1 == 1))
    {
        float LLC_Temp_V    = (float)(AdcRegs.ADCRESULT0 >> 4);
        float LLC_Temp_I    = (float)(AdcRegs.ADCRESULT1 >> 4);
        float LLC_Temp_Vin  = (float)(AdcRegs.ADCRESULT2 >> 4);
        float LLC_Temp_Iout = (float)(AdcRegs.ADCRESULT3 >> 4);

        /* 清SEQ1完成标志 */
        AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;

        /* IIR滤波 */
        LLC_Temp_V_    = (LLC_Temp_V    * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * LLC_Temp_V_;
        LLC_Temp_I_    = (LLC_Temp_I    * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * LLC_Temp_I_;
        LLC_Temp_Vin_  = (LLC_Temp_Vin  * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * LLC_Temp_Vin_;
        LLC_Temp_Iout_ = (LLC_Temp_Iout * ADC_Transform) * IIR_ALPHA + (1.0f - IIR_ALPHA) * LLC_Temp_Iout_;

        /* 工程量换算 */
        LLC_Values.Vout = LLC_Temp_V_;               // 别再写死100.0f
        LLC_Values.IL   = LLC_Temp_I_    * 44.4444f;
        LLC_Values.Vin  = LLC_Temp_Vin_  * 44.4444f;
        LLC_Values.Iout = LLC_Temp_Iout_ * 44.4444f;

        g_llc_adc_valid = 1;
    }

    /* 2. 启动“下一次”采样序列 */
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
    g_llc_adc_started = 1;
}

#pragma CODE_SECTION(IoutOffsetCal, ".TI.ramfunc")
/**
 * @brief 计算电流输出偏移
 * @param None
 * @return 如果校准完成则返回1，否则返回0
 */
char IoutOffsetCal(void)
{
    /* 电流偏移校准计数器 */
    static int Cnt=0;
    /* 偏移校准完成标志 */
    static  char Flag=0;
    /* 移动平均和 */
    static long IoutOffsetSum=0;

    /* 采样累加 */
    IoutOffsetSum += (AdcRegs.ADCRESULT3 >> 4);
    /* 偏移累加 */
    Cnt++;
    /* 累加256次 */
    if(Cnt>=256)
    {
        /* 清除累加计时器 */
        Cnt=0;
        /* 计算256个样本的平均值作为偏移 */
        Flag=1;
        LLC_Values.Iout_Offset = (float)(IoutOffsetSum >> 8);
        IoutOffsetSum = 0;
    }
    /* 返回偏移校准完成标志 */
    return Flag;
}
