#include "taskRecv.h"
#include "taskComm.h"

void
taskRecv(void *pvParameters)
{
    for (;;) {
        ucomm_Message msg;
        taskComm_Command cmd;

        ucomm_read(&msg);

        switch (msg.header.type) {
        case UCOMM_MESSAGE_SAMPLE_NACK:
            cmd.type = TASKCOMM_COMMAND_RESEND_SAMPLE;
            cmd.resendSample.packetTypes = msg.sampleNack.packetTypes;
            cmd.resendSample.id = msg.sampleNack.id;

            while (!xQueueSendToBack(taskComm_queue, &cmd, portMAX_DELAY));
            break;
        default:
            // ignore message
            break;
        }
    }
}
