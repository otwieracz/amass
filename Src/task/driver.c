/* file: Src/task/driver.c */

#include <stdio.h>
#include <string.h>

#include "stm32f3_discovery.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"

#include "error.h"
#include "hardware/hardware.h"
#include "task/driver.h"
#include "task/extctrl.h"

#define PRINT_TIMES 255

/* Select timer 3
 * PA0 TIM3_CH1
 * PA1 TIM3_CH2
 * PA2 TIM3_CH3 */

#define TIMX TIM3
#define GPIO_AF_TIMX GPIO_AF2_TIM3;
#define GPIOX GPIOC
#define __TIMX_CLK_ENABLE() (__TIM3_CLK_ENABLE())
#define __GPIOX_CLK_ENABLE() (__GPIOC_CLK_ENABLE())
#define GPIO_TIMX_CHANNEL_1 GPIO_PIN_6
#define GPIO_TIMX_CHANNEL_2 GPIO_PIN_7
#define GPIO_TIMX_CHANNEL_3 GPIO_PIN_8
void TIM3_IRQHandler(void);

TIM_HandleTypeDef  xDriverTimerHandle;
TIM_OC_InitTypeDef xDriverTimerPwmConf;

void prvTaskDriver(void* pvParameters);
void prvDriverHandler(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* CLI hook function and command definition */
static BaseType_t prvTaskDriverDebug(
        uint8_t *pcWriteBuffer,
        size_t xWriteBufferLen,
        const int8_t *pcCommandString);

static const CLI_Command_Definition_t xTaskDriverDebugCommand =
{
    "driver",
    "driver:\r\n Get current DRIVER readings\r\n\r\n",
    prvTaskDriverDebug,
    0
};
uint8_t ucDriverPrintTimes = 0;

/* Exported function, to start Driver */
void vTaskDriverStart(void)
{
    GPIO_InitTypeDef xGpioInitStruct;

    /* Enable timer clock adn GPIO */
    __TIMX_CLK_ENABLE();
    __GPIOX_CLK_ENABLE();

    /* Configure GPIO to alternate functions */
    //xGpioInitStruct.Pin       = (GPIO_TIMX_CHANNEL_1 | GPIO_TIMX_CHANNEL_2 | GPIO_TIMX_CHANNEL_3);
    xGpioInitStruct.Pin       = GPIO_TIMX_CHANNEL_2;
    xGpioInitStruct.Mode      = GPIO_MODE_AF_PP;
    xGpioInitStruct.Pull      = GPIO_PULLDOWN;
    xGpioInitStruct.Speed     = GPIO_SPEED_HIGH;
    xGpioInitStruct.Alternate = GPIO_AF_TIMX;
    HAL_GPIO_Init(GPIOX, &xGpioInitStruct);

    /* Configure timer
     * Should be:
     * freq   = 100kHz (100000)
     * 1 tick = 0.01ms
     * period = 20/0.01 = 20000 = 20ms
     *
     * but does not work. Multiply by 10 and woohoo. Do not ask :)
     */
    vHardwareTimerSetup(&xDriverTimerHandle, TIMX, 20000, 1000000);
    if(HAL_TIM_PWM_Init(&xDriverTimerHandle) != HAL_OK)
    {
        vErrorFatal(__LINE__, __FILE__, "prvTaskDriver: Unable to start TIM as PWM");
    }


    /* Setup PWM */
    /* TODO
     *
     * recieve messages with channel (pitch, roll, yaw) and set to pwm to given value
     * to control servo
     */
    vHardwareTimerPwmSetup(
            &xDriverTimerHandle,
            &xDriverTimerPwmConf,
            TIM_CHANNEL_2,
            TIM_OCPOLARITY_HIGH,
            1500);

    /* Create task and register CLI command */
    if(xTaskCreate(
                prvTaskDriver,
                "DRIVER",
                configMINIMAL_STACK_SIZE,
                NULL,
                1,
                NULL)
            != pdPASS)
        vErrorFatal(__LINE__, __FILE__, "prvTaskDriver: Unable to start");

    FreeRTOS_CLIRegisterCommand(&xTaskDriverDebugCommand);
}

void prvTaskDriver(void* pvParameters)
{
    uint16_t usX     = 1500;
    int8_t usDelta = 50;
    while(1)
    {
        xSemaphoreTake(xExtCtrlSem, 0);
        taskDISABLE_INTERRUPTS();
        vHardwareTimerPwmSetup(
                &xDriverTimerHandle,
                &xDriverTimerPwmConf,
                TIM_CHANNEL_2,
                TIM_OCPOLARITY_HIGH,
                (xExtCtrl.pitch*10)+1000);

        taskENABLE_INTERRUPTS();
        /* Critical section */
        xSemaphoreGive(xExtCtrlSem);

        HAL_Delay(20);
    }
}

/* CLI command for reading driver for PRINT_TIMES times */
static BaseType_t prvTaskDriverDebug(
        uint8_t *pcWriteBuffer,
        size_t xWriteBufferLen,
        const int8_t *pcCommandString)
{
    /* Zero buffer */
    memset(pcWriteBuffer, 0x00, xWriteBufferLen); 

    /* We assume that pcWriteBuffer is long enough, block on semaphore*/
    snprintf((char*)pcWriteBuffer,
            xWriteBufferLen,
            "DRIVER\r\n");
    /* End command */
    return pdFALSE;
}

