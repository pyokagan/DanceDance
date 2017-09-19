#include "ucomm.h"
#include <stdio.h>
#include <string.h>

#ifndef F
#define F(x) x
#endif

static const char *
typeStr(ucomm_type_t type)
{
    switch (type) {
    case UCOMM_MESSAGE_SAMPLE_NACK:
        return F("sample-nack");
    case UCOMM_MESSAGE_ACC1:
        return F("acc1");
    case UCOMM_MESSAGE_ACC1_RESEND:
        return F("acc1-resend");
    case UCOMM_MESSAGE_ACC2:
        return F("acc2");
    case UCOMM_MESSAGE_ACC2_RESEND:
        return F("acc2-resend");
    case UCOMM_MESSAGE_GYRO1:
        return F("gyro1");
    case UCOMM_MESSAGE_GYRO1_RESEND:
        return F("gyro1-resend");
    case UCOMM_MESSAGE_GYRO2:
        return F("gyro2");
    case UCOMM_MESSAGE_GYRO2_RESEND:
        return F("gyro2-resend");
    case UCOMM_MESSAGE_POW:
        return F("pow");
    default:
        return F("unknown");
    }
}


void
ucomm_print(const ucomm_Message *msg)
{
    char buf[64] = "";

    switch (msg->header.type) {
    case UCOMM_MESSAGE_SAMPLE_NACK:
        snprintf(buf, sizeof(buf), F("sample-nack, %u, %s\n"),
                msg->sampleNack.id, typeStr(msg->sampleNack.packetTypes));
        break;
    case UCOMM_MESSAGE_ACC1:
    case UCOMM_MESSAGE_ACC1_RESEND:
    case UCOMM_MESSAGE_ACC2:
    case UCOMM_MESSAGE_ACC2_RESEND:
        snprintf(buf, sizeof(buf), F("%s, %u, %d, %d, %d\n"),
                typeStr(msg->acc.type), msg->acc.id, msg->acc.x, msg->acc.y, msg->acc.z);
        break;
    case UCOMM_MESSAGE_GYRO1:
    case UCOMM_MESSAGE_GYRO1_RESEND:
    case UCOMM_MESSAGE_GYRO2:
    case UCOMM_MESSAGE_GYRO2_RESEND:
        snprintf(buf, sizeof(buf), F("%s, %u, %d, %d, %d\n"),
                typeStr(msg->gyro.type), msg->gyro.id, msg->gyro.x, msg->gyro.y, msg->gyro.z);
        break;
    case UCOMM_MESSAGE_POW:
        snprintf(buf, sizeof(buf), F("%s, %u, %u, %u\n"),
                typeStr(msg->pow.type), msg->pow.id, msg->pow.voltage, msg->pow.current);
        break;
    default:
        snprintf(buf, sizeof(buf), F("unknown\n"));
        break;
    }

    udebug_writeString(buf);
}
