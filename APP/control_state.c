#include "control_state.h"
#include "control_config.h"
#include "control_pwm.h"
#include "control_sampling.h"

typedef enum
{
    Init,
    Wait,
    Rise,
    Run,
    Err
} STATE_M;

typedef enum
{
    SSInit,
    SSWait,
    SSRun
} S_State_M;

#define LLC_Error_NOERR 0x0000
#define F_BURST         0x0002

static struct
{
    unsigned int StateM;
    unsigned int Ctr;
    unsigned int Error;
} LLC_Flag;

static S_State_M Soft_State_M = SSInit;
/* 供 CCS Graph 观察软启动子状态。 */
volatile unsigned int g_Soft_State_M_for_Graph = 0;
volatile unsigned char RsingInitFlag = 0;

#pragma CODE_SECTION(LLC_ValInit, ".TI.ramfunc")
/**
 * @brief 初始化LLC控制值
 * @param control 快慢环共享的控制指令
 * @return None
 */
static void LLC_ValInit(volatile struct _Ctr_values *control)
{
    /* 清除错误标志 */
    LLC_Flag.Error = 0;

    control->V_Ref       = 0.0f;
    /* 初始化周期时间为谐振频率点 (2.5us @ 400kHz) */
    control->Ctrl_CycleTime = LLC_RESONANT_CYCLE_US;
}

#pragma CODE_SECTION(LLC_StateMInit, ".TI.ramfunc")
/**
 * @brief 初始化状态处理函数
 * @param control 快慢环共享的控制指令
 * @return None
 */
static void LLC_StateMInit(volatile struct _Ctr_values *control)
{
    static char StateM_FirstFlag = 0;
    if(StateM_FirstFlag==0)
    {
        StateM_FirstFlag=1;
        LLC_ValInit(control);
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
static void LLC_StateMWait(void)
{
    static unsigned int LLC_CntS = 0;

    if (LLC_Flag.Error != LLC_Error_NOERR)
    {
        LLC_CntS = 0;
        return;
    }

    LLC_CntS++;

    if (LLC_CntS >= LLC_WAIT_TICKS)   /* 600 * 2.5ms = 1.5s */
    {
        LLC_CntS = 0;
        LLC_Flag.StateM = Rise;
        return;
    }
}

#pragma CODE_SECTION(LLC_StateMRise, ".TI.ramfunc")
/**
 * @brief 带子状态机的软启动状态处理函数
 * @param control 快慢环共享的控制指令
 * @return None
 */
static void LLC_StateMRise(volatile struct _Ctr_values *control)
{
    static  unsigned int  LLC_Rise_Cnt = 0;

    switch(Soft_State_M)
    {
        case SSInit:    /* 软启动初始状态 */
        {
            RsingInitFlag = 0;
            PWM_Disable();
            control->MAX_CycleTime  = LLC_MIN_CYCLE_US;  /* 软启动时使用最高频率 */
            control->Ctrl_DeadTime  = LLC_MAX_DEAD_TIME_NS;
            control->V_Ref          = LLC_SOFT_START_VREF;
            Soft_State_M            = SSWait;
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
            control->MAX_CycleTime += LLC_SOFT_START_STEP_US;
            control->Ctrl_CycleTime = control->MAX_CycleTime;

            /* 软启动期间逐渐减小死区时间 */
            control->Ctrl_DeadTime -= LLC_SOFT_START_STEP_NS;

            /* 限制最大周期时间 */
            if (control->MAX_CycleTime > LLC_MAX_CYCLE_US)
            {
                control->MAX_CycleTime = LLC_MAX_CYCLE_US;
            }

            /* 限制最小死区时间 */
            if (control->Ctrl_DeadTime < LLC_MIN_DEAD_TIME_NS)
            {
                control->Ctrl_DeadTime = LLC_MIN_DEAD_TIME_NS;
            }

            /* 当周期和死区都达到目标时，软启动完成 */
            if ((control->MAX_CycleTime >= LLC_MAX_CYCLE_US) &&
                (control->Ctrl_DeadTime <= LLC_MIN_DEAD_TIME_NS))
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
static void LLC_StateMRun(void)
{
    /* 正常运行 - 此处可添加电压/电流保护逻辑 */
}

#pragma CODE_SECTION(LLC_StateMErr, ".TI.ramfunc")
/**
 * @brief 错误状态处理函数
 * @param None
 * @return None
 */
static void LLC_StateMErr(void)
{
    PWM_Disable();
}

#pragma CODE_SECTION(LLC_State_M, ".TI.ramfunc")
/**
 * @brief LLC状态机主处理函数
 * @param control 快慢环共享的控制指令
 * @return None
 */
void LLC_State_M(volatile struct _Ctr_values *control)
{
    switch(LLC_Flag.StateM)
    {
        case  Init :LLC_StateMInit(control);    break;  /* 初始化状态 */
        case  Wait :LLC_StateMWait();           break;  /* 等待状态 */
        case  Rise :LLC_StateMRise(control);    break;  /* 软启动状态 */
        case  Run  :LLC_StateMRun();            break;  /* 运行状态 */
        case  Err  :LLC_StateMErr();            break;  /* 错误状态 */
    }
}

void LLC_BurstCtl(const volatile struct _Ctr_values *control)
{
    static unsigned char    BurstOffFlag    = 0;
    static unsigned int     BurstOnCnt      = 0;
    static unsigned int     BurstOffCnt     = 0;

    /* Burst模式仅在运行状态下允许 */
    if(LLC_Flag.StateM != Run)
    {
        BurstOnCnt = 0;
        BurstOffCnt = 0;
        BurstOffFlag = 0;
        return;
    }

    /* Burst模式被禁用 */
    if(!(LLC_Flag.Ctr & F_BURST))
    {
        BurstOnCnt  = 0;
        BurstOffCnt = 0;

        if(BurstOffFlag)
        {
            BurstOffFlag = 0;
            PWM_Enable();
        }
        return;
    }

    /* 进入Burst: 轻载 -> 更高频率 -> 更小周期时间 */
    if(control->Ctrl_CycleTime < BURST_ON_CYCLE_THRESHOLD)
    {
        BurstOffCnt = 0;

        if(BurstOnCnt < LLC_BURST_TICKS)
            BurstOnCnt++;

        if((BurstOnCnt >= LLC_BURST_TICKS) && (!BurstOffFlag))
        {
            BurstOffFlag = 1;
            PWM_Disable();
        }
    }
    /* 退出Burst */
    else if(control->Ctrl_CycleTime > BURST_OFF_CYCLE_THRESHOLD)
    {
        BurstOnCnt = 0;

        if(BurstOffCnt < LLC_BURST_TICKS)
            BurstOffCnt++;

        if((BurstOffCnt >= LLC_BURST_TICKS) && BurstOffFlag)
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
