/* file: Src/cli/tasks_info.c */

/* Tasks related CLI commands */

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"

#include "hardware/uart.h"

#include "cli/tasks_info.h"

static BaseType_t prvCliTasks(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);

static const CLI_Command_Definition_t xCliTasksCommand =
{
    "tasks",
    "tasks:\n Lists all tasks in RTOS (NOTE: will disable interrupts for its duration!)\r\n\r\n",
    prvCliTasks,
    0
};

void vCliTasksRegister(void)
{
    FreeRTOS_CLIRegisterCommand(&xCliTasksCommand);
}

static BaseType_t prvCliTasks(uint8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString)
{
    uint8_t numberOfTasks = uxTaskGetNumberOfTasks();
    char foo[512];

    vTaskList((char*)&foo);

    /* The entire table was written directly to the output buffer.  Execution
    of this command is complete, so return pdFALSE. */
    return pdFALSE;
}
