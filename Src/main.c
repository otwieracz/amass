/* main.c */

#include "stm32f3xx_hal.h"     /* HAL */
#include "stm32f3_discovery.h" /* BSP */

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

#include "error.h"
#include "hardware/hardware.h"

/* Tasks */
#include "task/cli.h"
#include "task/heartbeat.h"
#include "task/gyro.h"
#include "task/accel.h"
#include "task/attind.h"
#include "task/extctrl.h"

/* CLI */
#include "cli/tasks_info.h"

int main(void)
{
    /* Setup hardware */
    vHardwareSetup();

    /* Start tasks */
    vTaskCliStart();
    vTaskHeartbeatStart();
    vTaskGyroStart();
    vTaskAccelStart();
    vTaskAttindStart();
    vTaskExtCtrlStart();

    /* Register CLIs */
    vCliTasksRegister();

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

