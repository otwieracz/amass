/* file: Src/task/accel.h */

#ifndef _TASK_ACCEL_H
#define _TASK_ACCEL_H

#include "FreeRTOS.h"
#include "semphr.h"

#ifndef TASK_ACCEL_SAMPLING_RATE
/* Accelerometer sampling rate (times per second) */
#define TASK_ACCEL_SAMPLING_RATE 100
#endif /* TASK_ACCEL_SAMPLING_RATE */

/* Data struct */
typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} AccelData;

/* Data containers */
AccelData xAccel;

/* Semaphores */
SemaphoreHandle_t xAccelSem;

/* Exported functions */
void vTaskAccelStart(void);

#endif /* _TASK_ACCEL_H */

