/*
 * A program which prints all messages received over UART (/dev/serial0) to
 * stdout.
 */
#include <stdio.h>
#include <ucomm.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t done;

static void
onTerm(int signum)
{
    done = 1;
}

void
timespec_diff(const struct timespec *start, const struct timespec *stop,
        struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
}

int
main(int argc, char *argv[])
{
    ucomm_Message msg;
    bool firstMessage = true;
    int numDisconnects = 0;
    int numDropped = 0;
    uint8_t lastId; // Used to detect dropped messages
    struct timespec lastTime; // Used to detect disconnects

    userial_init();

    struct sigaction action = {};
    action.sa_handler = onTerm;
    sigaction(SIGINT, &action, NULL);

    while (!done) {
        ucomm_read(&msg);
        if (!firstMessage) {
            struct timespec currentTime, diff;

            clock_gettime(CLOCK_MONOTONIC, &currentTime);
            timespec_diff(&lastTime, &currentTime, &diff);
            if (diff.tv_sec >= 1) {
                // A disconnect occurred
                puts("DISCONNECT");
                numDisconnects++;
            } else {
                // Determine how many packets were dropped, taking into account
                // overflow.
                if (msg.header.id <= lastId) {
                    // Overflow
                    numDropped += 255 - lastId + msg.header.id;
                } else {
                    numDropped += msg.header.id - lastId - 1;
                }
            }
            lastTime = currentTime;
        } else {
            clock_gettime(CLOCK_MONOTONIC, &lastTime);
        }
        firstMessage = false;
        lastId = msg.header.id;

        switch (msg.header.type) {
        case UCOMM_MESSAGE_SAMPLE:
            printf("sample, %u, %d, %d, %d, %d, %d, %d\n", msg.header.id,
                    msg.sample.acc1x, msg.sample.acc1y, msg.sample.acc1z,
                    msg.sample.acc2x, msg.sample.acc2y, msg.sample.acc2z);
            break;
        default:
            printf("unknown, %u\n", msg.header.id);
            break;
        }
    }

    fprintf(stderr, "Number of dropped packets: %d\n", numDropped);
    fprintf(stderr, "Number of disconnects: %d\n", numDisconnects);

    return 0;
}
