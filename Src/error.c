/* file: error.c */

#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "error.h"
#include "hardware/hardware.h"
#include "hardware/uart.h"

/* 
 * Error handling function.
 *
 */

/* Signal fatal error by blinking all LEDs */
void vErrorFatalLoop(void)
{
    while(1)
    {
        /* Flash all LEDs */
        BSP_LED_Toggle(LED10);
        BSP_LED_Toggle(LED9);
        BSP_LED_Toggle(LED8);
        BSP_LED_Toggle(LED7);
        BSP_LED_Toggle(LED6);
        BSP_LED_Toggle(LED5);
        BSP_LED_Toggle(LED4);
        BSP_LED_Toggle(LED3);
        HAL_Delay(350);
    }
}

void vErrorFatal(uint16_t line, char* file, char* message)
{
    vTaskSuspend(NULL);
}

void vErrorWarning(uint16_t line, char* file, char* message)
{
}

