/* file: Src/task/uartrxtx.c */

#include <stdio.h>
#include <string.h>

#include "stm32f3_discovery.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "queue.h"

#include "error.h"
#include "hardware/uart.h"
#include "task/uartrxtx.h"

/* Array of subscring queues */
static QueueHandle_t* xSubscribers[UARTRXTX_MAX_SUBSCRIBERS];

/* TX queue */
QueueHandle_t xTxQueue;

void prvTaskUartRxTx(void* pvParameters);

/* Exported function, to start UartRxTx */
void vTaskUartRxTxStart(void)
{
    uint8_t i;

    /* Init subscribers array with NULL */
    for(i = 0; i < UARTRXTX_MAX_SUBSCRIBERS; i++)
    {
        xSubscribers[i] = NULL;
    }

    /* Create TX queue */
    xTxQueue = xQueueCreate(UARTRXTX_QUEUE_SIZE, sizeof(xUartRxTxMsg));

    /* Create task */
    if(xTaskCreate(
                prvTaskUartRxTx,
                "UARTRT",
                configMINIMAL_STACK_SIZE,
                NULL,
                1,
                NULL)
            != pdPASS)
        vErrorFatal(__LINE__, __FILE__, "prvTaskUartRxTx: Unable to start");
}

void prvTaskUartRxTx(void* pvParameters)
{
    BaseType_t   xReadState;
    xUartRxTxMsg xMsgBuffer;
    char         cRxedChar;
    uint8_t      i;

    while(1)
    {
        /* Recieve message and send over UART */
        if(xQueueReceive(xTxQueue, (void*)&xMsgBuffer, UARTRXTX_WAIT_TICKS))
        {
            xHardwareUartTx(xMsgBuffer.pcData, xMsgBuffer.xLength);
            vPortFree(xMsgBuffer.pcData);
        }

        /* Read char from UART if available */
        if(xHardwareUartRx(&cRxedChar, 1))
        {
            /* Send to subscribers */
            for(i = 0; i < UARTRXTX_MAX_SUBSCRIBERS; i++)
            {
                if(xSubscribers[i] != NULL)
                {
                    xQueueSendToBack(*(xSubscribers[i]), (void*)&cRxedChar, UARTRXTX_WAIT_TICKS);
                }
            }
        }
    }
}

/* Add new subscribing queue
 * Returns pdTRUE (if successful) or pdFALSE */
BaseType_t xTaskUartRxTxSub(QueueHandle_t* xRxQueue)
{
    uint8_t i;

    for(i = 0; i < UARTRXTX_MAX_SUBSCRIBERS; i++)
    {
        if(xSubscribers[i] == NULL)
        {
            xSubscribers[i] = xRxQueue;
            return pdTRUE;
        }
    }

    /* No more subscribers allowed */
    return pdFALSE;
}

/* Remove xRxQueue from subscribers list
 * Returns pdTRUE (if successful) or pdFALSE */
BaseType_t xTaskUartRxTxUnsub(QueueHandle_t* xRxQueue)
{
    uint8_t i;

    for(i = 0; i < UARTRXTX_MAX_SUBSCRIBERS; i++)
    {
        if(xSubscribers[i] == xRxQueue)
        {
            xSubscribers[i] = NULL;
            return pdTRUE;
        }
    }

    /* No such subscriber */
    return pdFALSE;
}


void vTaskUartRxTxSend(char* pcDataToSend, size_t xDataLen)
{
    xUartRxTxMsg xMsg;
    /* keep of stack */
    char* pcDataToSendCopy = (char*)pvPortMalloc(sizeof(char)*UARTRXTX_MAX_LEN);

    /* Make copy of pcDataToSend and make sure it ends with \0 */
    memcpy(pcDataToSendCopy, pcDataToSend, xDataLen);
    pcDataToSendCopy[UARTRXTX_MAX_LEN-1] = '\0';

    xMsg.pcData  = pcDataToSendCopy;
    xMsg.xLength = xDataLen;

    /* Free memory if not added to queue; else it will be freed later */
    if(!xQueueSendToBack(xTxQueue, (void*)&xMsg, UARTRXTX_WAIT_TICKS))
        vPortFree(pcDataToSendCopy);
}

