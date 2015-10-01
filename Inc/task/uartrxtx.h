/* file: task/uartrxtx.h */

#ifndef _TASK_UARTRXTX_H
#define _TASK_UARTRXTX_H

#include "queue.h"

/* Maximum length of string to send over UART */
#ifndef UARTRXTX_MAX_LEN
#define UARTRXTX_MAX_LEN 96
#endif

/* Number of characters to wait at most */
#ifndef UARTRXTX_QUEUE_SIZE
#define UARTRXTX_QUEUE_SIZE 128
#endif

/* Ticks to wait while receiving or sending received input to others */
#ifndef UARTRXTX_WAIT_TICKS
#define UARTRXTX_WAIT_TICKS 3
#endif

/* Maximum number of subscribers */
#ifndef UARTRXTX_MAX_SUBSCRIBERS
#define UARTRXTX_MAX_SUBSCRIBERS 16
#endif

/* TX struct */
typedef struct
{
    char*  pcData;
    size_t xLength;
} xUartRxTxMsg;

/* Command queue */
QueueHandle_t xUartRxTxQueue;

/* Exported function */
void       vTaskUartRxTxStart(void);
BaseType_t xTaskUartRxTxSub(QueueHandle_t* xRxQueue);
BaseType_t xTaskUartRxTxUnsub(QueueHandle_t* xRxQueue);
void       vTaskUartRxTxSend(char* xDataToSend, size_t xDataLen);

#endif /* _TASK_UARTRXTX_H */

