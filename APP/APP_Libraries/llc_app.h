#ifndef APP_LLC_APP_H_
#define APP_LLC_APP_H_

#include "DSP2833x_Device.h"

/* 初始化控制状态，读取初始 PWM 周期计数，或关闭 PWM 输出。 */
void LLC_App_Init(void);
Uint16 LLC_App_GetInitialTbprd(void);
void LLC_App_DisableOutput(void);

/* 快环由 Timer0 调用，慢环由 Timer1 调用。 */
void LLC_App_FastTick(void);
void LLC_App_SlowTick(void);

#endif /* APP_LLC_APP_H_ */
