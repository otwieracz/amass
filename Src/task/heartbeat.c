/* heartbeat.c */

#include "FreeRTOS.h"
#include "task.h"

#include "heartbeat.h"
#include "hardware.h"
#include "error.h"

void prvTaskHeartbeat(void *pvParams);

/* Run LEDs in circle */
void vTaskHeartbeatStart(void)
{
    if(xTaskCreate(prvTaskHeartbeat, (const signed char*)"prvTaskHeartbeat", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdPASS)
    {
        vErrorFatalLoop();
    }
}
void prvTaskHeartbeat(void *pvParams)
{
    while(1) {
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
