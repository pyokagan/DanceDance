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
    case UCOMM_MESSAGE_SAMPLE:
        msgsize = sizeof(ucomm_MessageSample);
        break;
    default:
        return;
    }

    uframe_write(msg, msgsize - sizeof(uint16_t));
}
