/*
 * A program which prints all messages received over UART (/dev/serial0) to
 * stdout.
 */
#include <stdio.h>
#include <ucomm.h>
#include <ucomm_SampleReader.h>
#include <ucomm_SampleAssembler.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t done;

static void
onTerm(int signum)
{
    done = 1;
}

int
main(int argc, char *argv[])
{
    ucomm_SampleReader sampleReader;
    ucomm_SampleAssembler sampleAssembler;

    userial_init();
    ucomm_SampleReader_init(&sampleReader);
    ucomm_SampleAssembler_init(&sampleAssembler, 63, 4);

    struct sigaction action = {};
    action.sa_handler = onTerm;
    sigaction(SIGINT, &action, NULL);

    while (!done) {
        ucomm_Message msg;

        ucomm_read(&msg);
        if (ucomm_SampleReader_feed(&sampleReader, &msg)) {
            if (sampleReader.disconnect) {
                ucomm_SampleAssembler_reset(&sampleAssembler);
                puts("DISCONNECT");
            }

            if (sampleReader.sampleReady) {
                printf("sample, %u, %d, %d, %d, %d, %d, %d\n",
                    sampleReader.sample.id,
                    sampleReader.sample.acc1.x, sampleReader.sample.acc1.y, sampleReader.sample.acc1.z,
                    sampleReader.sample.acc2.x, sampleReader.sample.acc2.y, sampleReader.sample.acc2.z);
            }

            ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
            if (sampleAssembler.ready) {
                printf("window\n");
            }
        } else {
            printf("unknown, %u\n", msg.header.type);
        }
    }

    fprintf(stderr, "Number of dropped samples: %d\n", sampleReader.numDropped);
    fprintf(stderr, "Number of disconnects: %d\n", sampleReader.numDisconnects);

    return 0;
}
