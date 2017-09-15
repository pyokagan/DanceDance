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
    ucomm_id_t sampleId = 0;

    for (;;) {
        taskComm_Command cmd;

        while (!xQueueReceive(taskComm_queue, &cmd, portMAX_DELAY));
        switch (cmd.type) {
        case TASKCOMM_COMMAND_SEND_SAMPLE:
            cmd.sendSample.sample.id = sampleId++;
            ucomm_writeSample(&cmd.sendSample.sample);
            break;
        }
    }
}
