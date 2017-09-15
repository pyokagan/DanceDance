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
        cmd.type = TASKCOMM_COMMAND_SEND_SAMPLE;
        cmd.sendSample.sample.acc1.x = 0;
        cmd.sendSample.sample.acc1.y = 1;
        cmd.sendSample.sample.acc1.z = 2;
        cmd.sendSample.sample.acc2.x = 3;
        cmd.sendSample.sample.acc2.y = 4;
        cmd.sendSample.sample.acc2.z = 5;

        while (!xQueueSendToBack(taskComm_queue, &cmd, portMAX_DELAY));

        vTaskDelayUntil(&lastWakeTime, 23 / portTICK_PERIOD_MS);
    }
}
