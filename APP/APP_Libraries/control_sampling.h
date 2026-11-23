#ifndef APP_CONTROL_SAMPLING_H_
#define APP_CONTROL_SAMPLING_H_

#include "control_types.h"

/* Returns NULL only until the first completed conversion has been read. */
const struct _ADC_Values *LLC_ADC_ReadPrevious_And_StartNext(void);
char IoutOffsetCal(void);

#endif /* APP_CONTROL_SAMPLING_H_ */
