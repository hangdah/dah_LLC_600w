#include "control_pwm.h"
#include "control_config.h"

static struct
{
    float PWM_CycleTime;  /* us */
    float PWM_DeadTime;   /* ns */
    Uint16 PWM_TBPRD;
    Uint16 PWM_AComp;
} LLC_PWM;

/**
 * @brief 初始化LLC PWM时序值
 * @param None
 * @return None
 */
void LLC_PWM_Values_Init(void)
{
    const float periodSeconds = 1.0f / LLC_RESONANT_FREQ;

    /* 初始周期计数直接截断取整，运行时更新采用四舍五入。 */
    LLC_PWM.PWM_TBPRD       = LLC_CPU_MHZ * 1000000.0f * 0.5f * periodSeconds;
    LLC_PWM.PWM_CycleTime   = LLC_RESONANT_CYCLE_US;
    LLC_PWM.PWM_AComp       = LLC_INITIAL_DUTY * LLC_PWM.PWM_TBPRD;

    /* 死区时间，单位 ns */
    LLC_PWM.PWM_DeadTime    = LLC_INITIAL_DEAD_TIME_NS;

    /* 将死区时间从 ns 转为 DBRED/DBFED 计数
       count = t_dead(ns) * TBCLK(Hz) / 1e9 */
    EALLOW;
    EPwm1Regs.DBRED         = (Uint16)(LLC_PWM.PWM_DeadTime * LLC_DEAD_COUNTS_PER_NS + 0.5f);
    EPwm1Regs.DBFED         = (Uint16)(LLC_PWM.PWM_DeadTime * LLC_DEAD_COUNTS_PER_NS + 0.5f);
    EDIS;
}

Uint16 LLC_PWM_GetTbprd(void)
{
    return LLC_PWM.PWM_TBPRD;
}

#pragma CODE_SECTION(LLC_PWM_Update, ".TI.ramfunc")
/**
 * @brief 根据PI输出更新LLC PWM频率
 * @param control 开关周期（us）和死区时间（ns）指令
 * @return None
 */
void LLC_PWM_Update(const volatile struct _Ctr_values *control)
{
    LLC_PWM.PWM_TBPRD = (Uint16)(LLC_TBPRD_PER_US * control->Ctrl_CycleTime + 0.5f);
    if (LLC_PWM.PWM_TBPRD < 2)
        LLC_PWM.PWM_TBPRD = 2;

    LLC_PWM.PWM_AComp = LLC_PWM.PWM_TBPRD >> 1;   /* 比较值取半周期，对应约 50% 占空比 */
    if (LLC_PWM.PWM_AComp < 1)
        LLC_PWM.PWM_AComp = 1;
    else if (LLC_PWM.PWM_AComp >= LLC_PWM.PWM_TBPRD)
        LLC_PWM.PWM_AComp = LLC_PWM.PWM_TBPRD - 1;

    LLC_PWM.PWM_CycleTime = control->Ctrl_CycleTime;
    LLC_PWM.PWM_DeadTime = control->Ctrl_DeadTime;

    if (LLC_PWM.PWM_DeadTime < 0.0f)
        LLC_PWM.PWM_DeadTime = 0.0f;

    EALLOW;
    EPwm1Regs.TBPRD = LLC_PWM.PWM_TBPRD;
    EPwm1Regs.CMPA.half.CMPA = LLC_PWM.PWM_AComp;
    EPwm1Regs.DBRED = (Uint16)(LLC_PWM.PWM_DeadTime * LLC_DEAD_COUNTS_PER_NS + 0.5f);
    EPwm1Regs.DBFED = (Uint16)(LLC_PWM.PWM_DeadTime * LLC_DEAD_COUNTS_PER_NS + 0.5f);
    EDIS;
}

#pragma CODE_SECTION(PWM_Disable, ".TI.ramfunc")
/* 强制触发一次性 Trip，将两路 PWM 输出保持为低电平。 */
void PWM_Disable(void)
{
    EALLOW;
    EPwm1Regs.TZFRC.bit.OST = 1;
    EDIS;
}

#pragma CODE_SECTION(PWM_Enable, ".TI.ramfunc")
/* 清除一次性 Trip 标志，以恢复 PWM 输出。 */
void PWM_Enable(void)
{
    EALLOW;
    EPwm1Regs.TZCLR.bit.OST = 1;
    EDIS;
}
