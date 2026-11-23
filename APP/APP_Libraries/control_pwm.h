#ifndef APP_CONTROL_PWM_H_
#define APP_CONTROL_PWM_H_

#include "DSP2833x_Device.h"
#include "control_types.h"

/* 初始化 PWM 时序，提供 TBPRD 计数，并按控制指令更新周期和死区。 */
void LLC_PWM_Values_Init(void);
Uint16 LLC_PWM_GetTbprd(void);
void LLC_PWM_Update(const volatile struct _Ctr_values *control);

/* 通过 ePWM1 的一次性 Trip 控制输出启停。 */
void PWM_Enable(void);
void PWM_Disable(void);

#endif /* APP_CONTROL_PWM_H_ */
