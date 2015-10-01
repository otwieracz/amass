/* file: Src/task/fd.c 
 *
 * Flight Director task
 *
 */

#include <stdio.h>
#include <string.h>

#include "stm32f3_discovery.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "error.h"
#include "task/fd.h"
#include "task/extctrl.h"
#include "task/gyro.h"
#include "task/driver.h"
#include "task/attind.h"

#define PRINT_TIMES 255

void prvTaskFd(void* pvParameters);

#define ABS(a) (((a) < 0) ? -(a) : (a))

/* CLI hook function and command definition */
static BaseType_t prvTaskFdDebug(
        uint8_t *pcWriteBuffer,
        size_t xWriteBufferLen, 
        const int8_t *pcCommandString
        );

static const CLI_Command_Definition_t xTaskFdDebugCommand =
{
    "fd",
    "fd:\r\n Get current FD decisions\r\n\r\n",
    prvTaskFdDebug,
    0
};
uint8_t ucFdPrintTimes = 0;

/* Exported function to start FD */
void vTaskFdStart(void)
{
    /* Create task and register CLI command */
    if(xTaskCreate(prvTaskFd, "FD", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL) != pdPASS)
        vErrorFatal(__LINE__, __FILE__, "prvTaskFd: Unable to start");
    FreeRTOS_CLIRegisterCommand(&xTaskFdDebugCommand);
}

void prvTaskFd(void* pvParameters)
{
    xDriverCmd xPitchAction = {DRIVER_PITCH, 0};
    xDriverCmd xRollAction = {DRIVER_ROLL, 0};
    xDriverCmd xYawAction = {DRIVER_YAW, 0};

    while(1)
    {
        /* Get EXTCTRL readings */
        xSemaphoreTake(xExtCtrlSem, 0);
        xPitchAction.sValue = xExtCtrl.pitch;
        xRollAction.sValue  = xExtCtrl.roll;
        xYawAction.sValue   = xExtCtrl.yaw;
        xSemaphoreGive(xExtCtrlSem);

        /* Continue processing of each axis only if there was no
         * certain input from EXTCTRL */

        if(ABS(xPitchAction.sValue) <= FD_EXTCTRL_DEAD_ZONE)
        {
            xPitchAction.sValue = 40;
        }

        if(ABS(xRollAction.sValue) <= FD_EXTCTRL_DEAD_ZONE)
        {
            xRollAction.sValue = 40;
        }

        if(ABS(xYawAction.sValue) <= FD_EXTCTRL_DEAD_ZONE)
        {
        }

        /* Send command to driver */
        xQueueSendToBack(xDriverQueue, (void*)&xPitchAction, 0);
        xQueueSendToBack(xDriverQueue, (void*)&xRollAction, 0);
        xQueueSendToBack(xDriverQueue, (void*)&xYawAction, 0);
        
        /* - read EXTCTRL
         * - check if ABS of any value > 5 (FD_EXTCTRL_DEAD_ZONE)
         * - - send to DRIVER
         * - */
    }
}

/* CLI command for reading gyro for PRINT_TIMES times */
static BaseType_t prvTaskFdDebug(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
    /* Zero buffer */
    memset(pcWriteBuffer, 0x00, xWriteBufferLen); 

//    if(ucFdPrintTimes == 0)
//        ucFdPrintTimes = PRINT_TIMES;
//
//    if(ucFdPrintTimes > 0)
//    {
//        /* We assume that pcWriteBuffer is long enough, block on semaphore*/
//        xSemaphoreTake(xFdSem, 0);
//        snprintf((char*)pcWriteBuffer,
//                xWriteBufferLen,
//                "%6d\t%6d\t%6d\r\n",
//                (int16_t)xFd.x,
//                (int16_t)xFd.y,
//                (int16_t)xFd.z);
//        xSemaphoreGive(xFdSem);
//        HAL_Delay(40);
//        ucFdPrintTimes--;
//    }
//
//    if(ucFdPrintTimes > 0)
//    {
//        /* Go for another loop */
//        return pdTRUE;
//    } else
//    {
        /* End command */
        return pdFALSE;
//    }
}
