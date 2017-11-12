#include <stdio.h>
#include <errno.h>
#include <ucomm.h>

int
main(int argc, char *argv[])
{
    ucomm_initRaspi();

    for (;;) {
        ucomm_Message msg;

        if (!ucomm_read(&msg)) {
            fprintf(stderr, "error: %s\n", strerror(errno));
            continue;
        }

        ucomm_print(&msg);
    }

    return 0;
}
