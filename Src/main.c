/* main.c */

#include "stm32f3xx_hal.h"     /* HAL */
#include "stm32f3_discovery.h" /* BSP */

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

#include "hardware.h"
#include "error.h"

/* Tasks */
#include "task/heartbeat.h"

int main(void)
{
    /* Setup hardware */
    vHardwareSetup();

    /* Start tasks */
    vTaskHeartbeatStart();

    /* Start RTOS scheduler */
	vTaskStartScheduler();

    /* We should never get here */
    while (1) {
        /* Signal error by flashing everything */
        vErrorFatalLoop();
    }

    /* We should never get here */
    return 0;
}

