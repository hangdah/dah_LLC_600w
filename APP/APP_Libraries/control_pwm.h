#ifndef APP_CONTROL_PWM_H_
#define APP_CONTROL_PWM_H_

#include "DSP2833x_Device.h"
#include "control_types.h"

void LLC_PWM_Values_Init(void);
Uint16 LLC_PWM_GetTbprd(void);
void LLC_PWM_Update(const volatile struct _Ctr_values *control);
void PWM_Enable(void);
void PWM_Disable(void);

#endif /* APP_CONTROL_PWM_H_ */
