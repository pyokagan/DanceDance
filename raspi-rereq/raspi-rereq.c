/*
 * A program which prints all messages received over UART (/dev/serial0) to
 * stdout.
 */
#include <stdio.h>
#include <ucomm.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    ucomm_Message msg;

    ucomm_init();

    // Send NACK
    msg.header.type = UCOMM_MESSAGE_NACK;
    msg.nack.id = 0;
    msg.nack.packetType = UCOMM_MESSAGE_ACC1;
    ucomm_write(&msg);

    for (;;) {
        ucomm_read(&msg);
        ucomm_print(&msg);
        if (msg.header.type == UCOMM_MESSAGE_ACC1 && msg.acc.id == 0)
            break;
    }

    return 0;
}
