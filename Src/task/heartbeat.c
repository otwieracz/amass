/* heartbeat.c */
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "error.h"
#include "task/heartbeat.h"
#include "hardware/hardware.h"

void vTaskHeartbeatStart(void);
void prvTaskHeartbeat(void *pvParameters);

/* Run LEDs in circle */
void vTaskHeartbeatStart(void)
{
    /* Initialize all configured peripherals */
    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);
    BSP_LED_Init(LED5);
    BSP_LED_Init(LED6);
    BSP_LED_Init(LED7);
    BSP_LED_Init(LED8);
    BSP_LED_Init(LED9);
    BSP_LED_Init(LED10);

    if(xTaskCreate(prvTaskHeartbeat, "tHBEAT", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdPASS)
    {
        vErrorFatal(__LINE__, __FILE__, "prvTaskHeartbeat: Unable to start");
    }
}

void prvTaskHeartbeat(void *pvParams)
{
    while(1)
    {
        BSP_LED_Toggle(LED3);
        HAL_Delay(100);
        BSP_LED_Toggle(LED5);
        HAL_Delay(100);
        BSP_LED_Toggle(LED7);
        HAL_Delay(100);
        BSP_LED_Toggle(LED9);
        HAL_Delay(100);
        BSP_LED_Toggle(LED10);
        HAL_Delay(100);
        BSP_LED_Toggle(LED8);
        HAL_Delay(100);
        BSP_LED_Toggle(LED6);
        HAL_Delay(100);
        BSP_LED_Toggle(LED4);
        HAL_Delay(100);
    }
}
