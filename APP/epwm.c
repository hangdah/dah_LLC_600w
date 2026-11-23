/**
 * @file epwm.c
 * @brief LLC谐振变换器控制的ePWM驱动
 * @date 2025
 * @author da
 *
 * ePWM1配置:
 * - 增减计数模式用于对称PWM
 * - 半桥驱动的死区时间插入
 * - 在谷值和峰值处触发ADC采样的SOC
 * - 硬件保护的触发区
 */

#include "epwm.h"
#include "exit.h"
#include "led.h"
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "SFO_V5.h"

int MEP_ScaleFactor[PWM_CH] = {0};
int SFO_Status = 0;

// SFO V5: ePWM register pointer array
volatile struct EPWM_REGS *ePWM[PWM_CH] =
{
    0,          // Dummy element
    &EPwm1Regs,
    &EPwm2Regs,
    &EPwm3Regs,
    &EPwm4Regs,
    &EPwm5Regs,
    &EPwm6Regs
};

#pragma CODE_SECTION(EPwm1A_SetCompare, ".TI.ramfunc")

/**
 * @brief 设置ePWM1A的比较值
 * @param val 比较值 (0-TBPRD)
 * @return None
 */
void EPwm1A_SetCompare(Uint16 val)
{
    EPwm1Regs.CMPA.half.CMPA = val;  /* 设置占空比 */
}

void EPwm1B_SetCompare(Uint16 val)
{
    EPwm1Regs.CMPB = val;
}

void EPwm2A_SetCompare(Uint16 val)
{
    EPwm2Regs.CMPA.half.CMPA = val;
}

void EPwm2B_SetCompare(Uint16 val)
{
    EPwm2Regs.CMPB = val;
}

void EPwm3A_SetCompare(Uint16 val)
{
    EPwm3Regs.CMPA.half.CMPA = val;
}

void EPwm3B_SetCompare(Uint16 val)
{
    EPwm3Regs.CMPB = val;
}

void EPwm4A_SetCompare(Uint16 val)
{
    EPwm4Regs.CMPA.half.CMPA = val;
}

void EPwm4B_SetCompare(Uint16 val)
{
    EPwm4Regs.CMPB = val;
}

void EPwm5A_SetCompare(Uint16 val)
{
    EPwm5Regs.CMPA.half.CMPA = val;
}

void EPwm5B_SetCompare(Uint16 val)
{
    EPwm5Regs.CMPB = val;
}

void EPwm6A_SetCompare(Uint16 val)
{
    EPwm6Regs.CMPA.half.CMPA = val;
}

void EPwm6B_SetCompare(Uint16 val)
{
    EPwm6Regs.CMPB = val;
}

/**
 * @brief 初始化ePWM1模块
 * @param tbprd 定时器周期值 (TBPRD)
 * @return None
 *
 * 为LLC半桥控制配置ePWM1:
 * - 增减计数模式
 * - 高电平有效互补带死区时间
 * - 用于ADC采样的SOC触发
 * - 触发区保护
 */
void EPWM1_Init(Uint16 tbprd){

    InitEPwm1Gpio();

    /* 时基模块配置 */
    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;
    EPwm1Regs.TBPHS.half.TBPHS = 0;
    EPwm1Regs.TBCTR = 0x0000;
    EPwm1Regs.TBPRD = tbprd;
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  /* 增减计数模式 */
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;

    /* 计数比较模块配置 */
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     /* CMPA影子模式 */
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     /* CMPB影子模式 */
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   /* CTR=0时加载 */
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   /* CTR=0时加载 */

    /* 动作限定器配置 - 互补PWM */
    EPwm1Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;            /* 增计数比较时清零 */
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;              /* 减计数比较时置位 */

    EPwm1Regs.AQCTLB.bit.ZRO = AQ_NO_ACTION;
    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;
    EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;

    /* 死区时间模块配置 */
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  /* 在双输出上使能死区 */
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       /* 高电平有效互补 */
    EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;

    /* 触发区配置 - 硬件保护 */
    EPwm1Regs.TZSEL.bit.OSHT1 = TZ_ENABLE;          /* TZ1上的一次性触发 */
    EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO;          /* 故障时强制PWM1A为低 */
    EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO;          /* 故障时强制PWM1B为低 */
    EPwm1Regs.TZCLR.all = 0xffff;                   /* 清除所有TZ标志 */
    EPwm1Regs.TZEINT.all = 0;                       /* 禁用TZ中断 */

    /* 事件触发配置 - 用于ADC采样的SOC */
//    EPwm1Regs.ETSEL.bit.SOCAEN = 0;                 /* 使能SOCA事件 */
//    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;      /* TBCTR=0时触发 (谷值) */
//    EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;            /* 每个事件触发一次 */
//
//    EPwm1Regs.ETSEL.bit.SOCBEN = 0;                 /* 使能SOCB事件 */
//    EPwm1Regs.ETSEL.bit.SOCBSEL = ET_CTR_PRD;       /* TBCTR=TBPRD时触发 (峰值) */
//    EPwm1Regs.ETPS.bit.SOCBPRD = ET_1ST;

    /* HRPWM 配置 */
    EALLOW;

    EPwm1Regs.HRCNFG.all = 0x0000;
    EPwm1Regs.HRCNFG.bit.EDGMODE = HR_FEP;          /* 下降沿高分辨率控制 */
    EPwm1Regs.HRCNFG.bit.CTLMODE = HR_CMP;          /* 通过 CMPAHR 控制 MEP */
    EPwm1Regs.HRCNFG.bit.HRLOAD = HR_CTR_ZERO;      /*  CTR=ZERO 时加载 */

    EDIS;
}

/**
 * @brief ePWM1中断服务程序 (当前未使用)
 * @param None
 * @return None
 */
interrupt void epwm1_isr(void){
    /* 清除中断标志 */
    EPwm1Regs.ETCLR.bit.INT = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

void HRPWM1_SFO_Init(void)
{
    // =====================================
    // 1. Obtain initial MEP scale factor
    //    HRPWM must be disabled here
    // =====================================
    while(SFO_MepDis_V5(1) == 0);

    // Initialize seed value
    MEP_ScaleFactor[0] = MEP_ScaleFactor[1];


    // =====================================
    // 2. Enable HRPWM
    // =====================================
    EALLOW;

    EPwm1Regs.HRCNFG.all = 0x0000;

    EPwm1Regs.HRCNFG.bit.EDGMODE = HR_FEP;
    EPwm1Regs.HRCNFG.bit.CTLMODE = HR_CMP;
    EPwm1Regs.HRCNFG.bit.HRLOAD = HR_CTR_ZERO;

    EDIS;


    // =====================================
    // 3. Enable SFO calibration
    // =====================================
    do
    {
        SFO_Status = SFO_MepEn_V5(1);

    }while(SFO_Status == 0);

    if(SFO_Status == 2)
    {
        // MEP scale factor error
        // Handle error here
    }
}

#pragma CODE_SECTION(EPWM1_SetCompareHR, ".TI.ramfunc")
void EPWM1_SetCompareHR(Uint16 cmpa, float cmpa_frac)
{
    Uint16 cmpahr;
    Uint32 cmpValue;

    // Check SFO calibration
    if(MEP_ScaleFactor[1] <= 0)
        return;

    // Limit fractional part
    if(cmpa_frac < 0.0f)
        cmpa_frac = 0.0f;

    if(cmpa_frac >= 1.0f)
    {
        cmpa++;
        cmpa_frac -= 1.0f;
    }

    // Calculate MEP steps
    cmpahr = (Uint16)(
        cmpa_frac * (float)MEP_ScaleFactor[1] + 1.5f
    );

    // Limit MEP value
    if(cmpahr > 255)
        cmpahr = 255;

    // Combine CMPA and CMPAHR
    cmpValue = ((Uint32)cmpa << 16)
             | ((Uint32)cmpahr << 8);

    // Update HRPWM compare register
    EPwm1Regs.CMPA.all = cmpValue;
}
