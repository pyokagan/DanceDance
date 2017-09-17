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
    int16_t i = 0;

    for (;;) {
        taskComm_Command cmd;
        cmd.type = TASKCOMM_COMMAND_SEND_SAMPLE;
        cmd.sendSample.sample.acc1.x = i++;
        cmd.sendSample.sample.acc1.y = i++;
        cmd.sendSample.sample.acc1.z = i++;
        cmd.sendSample.sample.acc2.x = i++;
        cmd.sendSample.sample.acc2.y = i++;
        cmd.sendSample.sample.acc2.z = i++;

        while (!xQueueSendToBack(taskComm_queue, &cmd, portMAX_DELAY));

        vTaskDelayUntil(&lastWakeTime, 23 / portTICK_PERIOD_MS);
    }
}
