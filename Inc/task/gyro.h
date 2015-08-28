/* file: task/gyro.h */

#ifndef _TASK_GYRO_H
#define _TASK_GYRO_H

#include "semphr.h"

#ifndef TASK_GYRO_SAMPLING_RATE
/* Gyro sampling rate (times per second) */
#define TASK_GYRO_SAMPLING_RATE 100
#endif /* TASK_GYRO_SAMPLING_RATE */

/* Data struct */
typedef struct
{
    float x;
    float y;
    float z;
} GyroData;

/* Data container */
GyroData xGyroDps;

/* Semaphores */
SemaphoreHandle_t xGyroDpsSem;

/* Exported function */
void vTaskGyroStart(void);

#endif /* _TASK_GYRO_H */
