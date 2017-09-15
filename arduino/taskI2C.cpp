#include "taskI2C.h"
#include "taskComm.h"

void
taskI2C_setup()
{
    // do nothing (for now)
}

void
taskI2C(void *pvParameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;) {
        taskComm_Command cmd;
        cmd.type = TASKCOMM_COMMAND_SEND;
        cmd.send.msg.header.type = UCOMM_MESSAGE_SAMPLE;
        cmd.send.msg.sample.acc1x = 0;
        cmd.send.msg.sample.acc1y = 1;
        cmd.send.msg.sample.acc1z = 2;
        cmd.send.msg.sample.acc2x = 3;
        cmd.send.msg.sample.acc2y = 4;
        cmd.send.msg.sample.acc2z = 5;

        while (!xQueueSendToBack(taskComm_queue, &cmd, portMAX_DELAY));

        vTaskDelayUntil(&lastWakeTime, 23 / portTICK_PERIOD_MS);
    }
}
