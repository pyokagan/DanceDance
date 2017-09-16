#include "ucomm.h"

void
ucomm_init(void)
{
    userial_init();
}

void
ucomm_read(ucomm_Message *msg)
{
    uframe_read(msg, sizeof(ucomm_Message));
}

void
ucomm_write(const ucomm_Message *msg)
{
    uint8_t msgsize;

    switch (msg->header.type) {
    case UCOMM_MESSAGE_SAMPLE_NACK:
        msgsize = sizeof(ucomm_MessageSampleNack);
        break;
    case UCOMM_MESSAGE_ACC1:
    case UCOMM_MESSAGE_ACC2:
        msgsize = sizeof(ucomm_MessageAcc);
        break;
    default:
        return;
    }

    uframe_write(msg, msgsize - sizeof(uint16_t));
}

void
ucomm_writeSample(const ucomm_Sample *sample)
{
    ucomm_writeSampleAcc1(sample);
    ucomm_writeSampleAcc2(sample);
}

void
ucomm_writeSampleAcc1(const ucomm_Sample *sample)
{
    ucomm_Message msg;

    msg.acc.type = UCOMM_MESSAGE_ACC1;
    msg.acc.id = sample->id;
    msg.acc.x = sample->acc1.x;
    msg.acc.y = sample->acc1.y;
    msg.acc.z = sample->acc1.z;
    ucomm_write(&msg);
}

void
ucomm_writeSampleAcc2(const ucomm_Sample *sample)
{
    ucomm_Message msg;

    msg.acc.type = UCOMM_MESSAGE_ACC2;
    msg.acc.id = sample->id;
    msg.acc.x = sample->acc2.x;
    msg.acc.y = sample->acc2.y;
    msg.acc.z = sample->acc2.z;
    ucomm_write(&msg);
}
