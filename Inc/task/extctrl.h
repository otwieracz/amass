/* file: task/extctrl.h */

#ifndef _TASK_EXTCTRL_H
#define _TASK_EXTCTRL_H

#include "semphr.h"

#ifndef TASK_EXTCTRL_SAMPLING_RATE
/* ExtCtrl sampling rate (times per second) */
#define TASK_EXTCTRL_SAMPLING_RATE 2000
#endif /* TASK_EXTCTRL_SAMPLING_RATE */

/* Data struct */
typedef struct
{
    uint8_t pitch;
    uint8_t roll;
    uint8_t yaw;
} ExtCtrlData;

/* Data container */
ExtCtrlData xExtCtrl;

/* Semaphores */
SemaphoreHandle_t xExtCtrlSem;

/* Exported function */
void vTaskExtCtrlStart(void);

#endif /* _TASK_EXTCTRL_H */
