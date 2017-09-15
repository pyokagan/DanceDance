#include "taskComm.h"

QueueHandle_t taskComm_queue;

void
taskComm_setup()
{
    ucomm_init();
    taskComm_queue = xQueueCreate(5, sizeof(taskComm_Command));
}

void
taskComm(void *pvParameters)
{
    ucomm_id_t msgid = 0;

    for (;;) {
        taskComm_Command cmd;

        while (!xQueueReceive(taskComm_queue, &cmd, portMAX_DELAY));
        switch (cmd.type) {
        case TASKCOMM_COMMAND_SEND:
            cmd.send.msg.header.id = msgid++;
            ucomm_write(&cmd.send.msg);
            break;
        }
    }
}
