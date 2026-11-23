/**
 * @file mppt.c
 * @brief 最大功率点跟踪 (MPPT) 算法实现
 * @date 2025
 * @author da
 *
 * 本文件实现了用于太阳能光伏应用的扰动观察法 (P&O) MPPT算法。
 * 支持基于电流 (Impp) 和基于电压 (Vmpp) 的MPPT控制模式。
 */

#include "mppt.h"

/**
 * @brief 初始化基于电流的MPPT控制器
 * @param v 指向MPPT电流控制器结构的指针
 * @return None
 */
void uinv_dcdc_mppt_init_I(uinv_dcdc_mppt_t_I *v) {
    v->mppt_first = 1;
}

/**
 * @brief 初始化基于电压的MPPT控制器
 * @param v 指向MPPT电压控制器结构的指针
 * @return None
 */
void uinv_dcdc_mppt_init_V(uinv_dcdc_mppt_t_V *v) {
    v->mppt_first = 1;
}

/**
 * @brief 运行基于电流的MPPT算法 (Impp跟踪)
 * @param v 指向MPPT电流控制器结构的指针
 * @return None
 *
 * 基于电流的扰动观察算法:
 * - 如果 dI > 0 且 dP > 0: 增加电流
 * - 如果 dI > 0 且 dP < 0: 减小电流
 * - 如果 dI < 0 且 dP > 0: 减小电流
 * - 如果 dI < 0 且 dP < 0: 增加电流
 */
void uinv_dcdc_mppt_run_Impp(uinv_dcdc_mppt_t_I *v) {
    float P = v->Vpv * v->Ipv;

    if(v->mppt_enable) {
        if(v->mppt_first) {
            v->ImppOut= v->Ipv;
            v->PpvOld = P;
            v->IpvOld = v->Ipv;
            v->mppt_first = 0;
        } else {
            float deltaI = v->Ipv - v->IpvOld;

            if(deltaI > 0) { /* 如果我们正在增加电流 */
                if(P > v->PpvOld) { /* 并且功率增加 */
                    v->ImppOut=v->Ipv+v->Stepsize;
                } else { /* 并且功率减小 */
                    v->ImppOut=v->Ipv-v->Stepsize;
                }
            } else { /* 如果我们正在减小电流 */
                if(P > v->PpvOld) { /* 并且功率增加 */
                    v->ImppOut=v->Ipv-v->Stepsize;
                } else { /* 并且功率减小 */
                    v->ImppOut=v->Ipv+v->Stepsize;
                }
            }
        }
    }
    /* 将输出钳位在安全范围内 */
    v->ImppOut=(v->ImppOut<v->MinI) ? v->MinI : v->ImppOut;
    v->ImppOut=(v->ImppOut>v->MaxI) ? v->MaxI : v->ImppOut;

    v->IpvOld = v->Ipv;
    v->PpvOld = P;
}

/**
 * @brief 运行基于电压的MPPT算法 (Vmpp跟踪)
 * @param v 指向MPPT电压控制器结构的指针
 * @return None
 *
 * 基于电压的扰动观察算法:
 * - 如果 dV > 0 且 dP > 0: 增加电压
 * - 如果 dV > 0 且 dP < 0: 减小电压
 * - 如果 dV < 0 且 dP > 0: 减小电压
 * - 如果 dV < 0 且 dP < 0: 增加电压
 */
void uinv_dcdc_mppt_run_Vmpp(uinv_dcdc_mppt_t_V *v) {
    float P = v->Vpv * v->Ipv;

    if(v->mppt_enable) {
        if(v->mppt_first) {
            v->VmppOut = v->Vpv;
            v->PpvOld = P;
            v->VpvOld = v->Vpv;
            v->mppt_first = 0;
        } else {
            float deltaV = v->Vpv - v->VpvOld;

            if(deltaV > 0) { /* 电压增加 */
                if(P > v->PpvOld) { /* 功率增加 */
                    v->VmppOut = v->Vpv + v->Stepsize;
                } else {
                    v->VmppOut = v->Vpv - v->Stepsize;
                }
            } else { /* 电压减小 */
                if(P > v->PpvOld) {
                    v->VmppOut = v->Vpv - v->Stepsize;
                } else {
                    v->VmppOut = v->Vpv + v->Stepsize;
                }
            }
        }
    }

    /* 将输出钳位在安全范围内 */
    v->VmppOut = (v->VmppOut < v->MinV) ? v->MinV : v->VmppOut;
    v->VmppOut = (v->VmppOut > v->MaxV) ? v->MaxV : v->VmppOut;

    v->VpvOld = v->Vpv;
    v->PpvOld = P;
}
