/*
 * A program which prints all frames received over UART (/dev/serial0) to
 * stdout.
 */
#include <stdio.h>
#include <ucomm.h>
#include <stdint.h>

int
main(int argc, char *argv[])
{
    uint8_t buf[128];

    userial_init();

    for (;;) {
        uint8_t nbytes = uframe_read(buf, sizeof(buf));
        fwrite(buf, 1, nbytes, stdout);
    }
}
