#include "ucomm.h"

void
ucomm_init(userial_write_t write, userial_flush_t flush, userial_read_t read)
{
    userial_write = write;
    userial_flush = flush;
    userial_read = read;
}

void
ucomm_initRaspi(void)
{
#ifndef ARDUINO
    userial_initRaspi();
#endif
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
    case UCOMM_MESSAGE_ACC1_RESEND:
    case UCOMM_MESSAGE_ACC2:
    case UCOMM_MESSAGE_ACC2_RESEND:
        msgsize = sizeof(ucomm_MessageAcc);
        break;
    case UCOMM_MESSAGE_POW:
        msgsize = sizeof(ucomm_MessagePow);
        break;
    default:
        return;
    }

    uframe_write(msg, msgsize - sizeof(uint16_t));
}

void
ucomm_writeSample(const ucomm_Sample *sample)
{
    ucomm_writeSampleAcc1(sample, false);
    ucomm_writeSampleAcc2(sample, false);
}

void
ucomm_writeSampleAcc1(const ucomm_Sample *sample, bool resend)
{
    ucomm_Message msg;

    msg.acc.type = resend ? UCOMM_MESSAGE_ACC1_RESEND : UCOMM_MESSAGE_ACC1;
    msg.acc.id = sample->id;
    msg.acc.x = sample->acc1.x;
    msg.acc.y = sample->acc1.y;
    msg.acc.z = sample->acc1.z;
    ucomm_write(&msg);
}

void
ucomm_writeSampleAcc2(const ucomm_Sample *sample, bool resend)
{
    ucomm_Message msg;

    msg.acc.type = resend ? UCOMM_MESSAGE_ACC2_RESEND : UCOMM_MESSAGE_ACC2;
    msg.acc.id = sample->id;
    msg.acc.x = sample->acc2.x;
    msg.acc.y = sample->acc2.y;
    msg.acc.z = sample->acc2.z;
    ucomm_write(&msg);
}

void
ucomm_writePow(const ucomm_Pow *pow)
{
    ucomm_Message msg;

    msg.pow.type = UCOMM_MESSAGE_POW;
    msg.pow.id = pow->id;
    msg.pow.voltage = pow->voltage;
    msg.pow.current = pow->current;
    ucomm_write(&msg);
}
