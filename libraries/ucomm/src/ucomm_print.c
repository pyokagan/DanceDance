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
    case UCOMM_MESSAGE_ACC2:
        return F("acc2");
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
    case UCOMM_MESSAGE_ACC2:
        snprintf(buf, sizeof(buf), F("%s, %u, %d, %d, %d\n"),
                typeStr(msg->acc.type), msg->acc.id, msg->acc.x, msg->acc.y, msg->acc.z);
        break;
    default:
        snprintf(buf, sizeof(buf), F("unknown\n"));
        break;
    }

    udebug_writeString(buf);
}
