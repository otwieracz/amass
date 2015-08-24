/* file: Src/cli/tasks_info.c */

/* Tasks related CLI commands */


#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"

#include "hardware/uart.h"
#include "cli/tasks_info.h"

#include <string.h>

/* Intermediate buffer between text-generating functions
 * and CLI output buffer */
char* pcIntermediateBuffer = NULL;
/* pcIntermediateBuffer will be moved as data is being sent in chunks,
 * pcIntermediateBufferHead will point at head to allow freeing */
char* pcIntermediateBufferHead = NULL;

static BaseType_t prvCliTaskList(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);

static const CLI_Command_Definition_t xCliTaskListCommand =
{
    "task_list",
    "task_list:\r\n Lists all tasks in RTOS (disable INT for runtime!)\r\n\r\n",
    prvCliTaskList,
    0
};

void vCliTasksRegister(void)
{
    FreeRTOS_CLIRegisterCommand(&xCliTaskListCommand);
}

static BaseType_t prvCliTaskList(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
    /* If pcIntermediateBuffer is NULL, then this is first execution 
     * of prvCliTasks. Else - this is one of following executions, 
     * writing output to pcWriteBuffer in slices */
    if(pcIntermediateBuffer == NULL)
    {
        static char* pcListHeader = "Name\tState\tPrio\tStack\tNum\r\n*******************************************\r\n";
        uint8_t numberOfTasks = uxTaskGetNumberOfTasks();
        /* according to FreeRTOS manual, 40 bytes per task should be enough.
         * Here we take 50 bytes per task.
         *
         * note about heap_2:
         * Heap 2 should not be used for allocations of random sizes. However, numberOfTasks
         * will be more or less constant - so we try to allocate it dynamically
         *
         * TODO: This may cause heap fragmentation
         */

        pcIntermediateBuffer     = pvPortMalloc(50*numberOfTasks + strlen(pcListHeader));
        pcIntermediateBufferHead = pcIntermediateBuffer;

        /* Prepare whole output */
        /* Write list header */
        strcpy(pcIntermediateBuffer, pcListHeader);
        /* Generate tasks list and put into output buffer, after header. */
        vTaskList((char*)pcIntermediateBuffer+strlen(pcListHeader));

        /* pcIntermediateBuffer contains complete output to print */
    }

    /* Zero pcWriteBuffer */
    memset(pcWriteBuffer, 0x00, xWriteBufferLen); 

    /* Send pcIntermediateBuffer in parts of xWriteBufferLen size */
    if(strlen(pcIntermediateBuffer) > 0)
    {
        /* Copy chunk from pcIntermediateBuffer to pcWriteBuffer */
        strncpy((char*)pcWriteBuffer, pcIntermediateBuffer, xWriteBufferLen-1);
        /* Move pcIntermediateBuffer pointer after data already sent */
        pcIntermediateBuffer += strlen((char*)pcWriteBuffer); 

        /* Output not completed - return pdTRUE to call this function again */
        return pdTRUE;
    } else
    {
        /* Output is completed */
        /* Free pcIntermediateBuffer(Head) */
        vPortFree(pcIntermediateBufferHead);

        /* Set both pointers to NULL */
        pcIntermediateBuffer     = NULL;
        pcIntermediateBufferHead = NULL;

        /* The entire table was written to the output buffer.  Execution
           of this command is completed, so return pdFALSE. */
        return pdFALSE;
    }
}

