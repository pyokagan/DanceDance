#include "taskPow.h"
#include "taskComm.h"

void
taskPow(void *pvParameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();
    taskComm_Command cmd;
    cmd.type = TASKCOMM_COMMAND_SEND_POW;
    cmd.sendPow.pow.voltage = 0;
    cmd.sendPow.pow.current = 0;

    for (;;) {
        // Read the raw value from the INA169 board.
        // Current (in Amperes) will be calculated on the raspberry pi
        cmd.sendPow.pow.current = analogRead(A0);

        while (!xQueueSendToBack(taskComm_queue, &cmd, portMAX_DELAY));

        // Send at 5Hz to account for possible packet loss
        vTaskDelayUntil(&lastWakeTime, 200 / portTICK_PERIOD_MS);
    }
}
