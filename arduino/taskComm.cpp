#include "taskComm.h"
#include "SampleBuffer.h"
#include "uart1.h"
#include <stdbool.h>

static SampleBuffer sampleBuffer;
QueueHandle_t taskComm_queue;

void
taskComm_setup()
{
    uart1_init();
    ucomm_init(uart1_write, NULL, uart1_read);
    taskComm_queue = xQueueCreate(100, sizeof(taskComm_Command));
    SampleBuffer_init(&sampleBuffer);
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
            SampleBuffer_put(&sampleBuffer, &cmd.sendSample.sample);
            break;
        case TASKCOMM_COMMAND_RESEND_SAMPLE: {
            uint8_t idx;

            if (!SampleBuffer_getSampleIdx(&sampleBuffer, cmd.resendSample.id, &idx))
                break;

            if (cmd.resendSample.packetTypes & UCOMM_SAMPLENACK_ACC1)
                ucomm_writeSampleAcc1(&sampleBuffer.sample[idx], true);

            if (cmd.resendSample.packetTypes & UCOMM_SAMPLENACK_ACC2)
                ucomm_writeSampleAcc2(&sampleBuffer.sample[idx], true);

            } break;
        }
    }
}
