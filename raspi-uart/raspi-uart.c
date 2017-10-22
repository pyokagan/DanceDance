#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <ucomm.h>
#include <ucomm_SampleAssembler.h>
#include <ucomm_PowReader.h>
#include "die.h"
#include "StrBuf.h"

static void
info(const char *fmt, ...)
{
    char buf[1024];
    va_list va;

    va_start(va, fmt);
    vsnprintf(buf, sizeof(buf), fmt, va);
    va_end(va);

    fprintf(stderr, "%s\n", buf);
}

static int
write_full(int fd, const char *buf, size_t count)
{
    while (count) {
        ssize_t nbytes = write(fd, buf, count);
        if (nbytes < 0) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            return -1;
        }
        count -= nbytes;
        buf += nbytes;
    }
    return 0;
}

static volatile sig_atomic_t done;

static void
onTerm(int signum)
{
    done = 1;
}

static bool streamMode = false;

static pthread_mutex_t samplesMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t samplesStatusCondition = PTHREAD_COND_INITIALIZER;
static unsigned int numSamples = 63;
static unsigned int slack = 2;
static int packetLoss = 0;
static enum {
    SAMPLES_OUTDATED,
    SAMPLES_READY,
    SAMPLES_READY_WITH_DISCONNECT,
    SAMPLES_QUIT
} samplesStatus = SAMPLES_OUTDATED;
static ucomm_Sample *samples;
char mlPipe[512] = "/dev/stdout";

static void *
samplePrinter(void *arg)
{
    StrBuf sb = STRBUF_INIT;
    int fd;
    bool firstSample;

reset:
    firstSample = true;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    fd = open(mlPipe, O_CREAT | O_APPEND | O_WRONLY, 0666);
    if (fd < 0)
        die("failed to open %s: %s", mlPipe, strerror(errno));
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    for (;;) {
        // Wait for timeslice to be ready
        pthread_mutex_lock(&samplesMutex);
        if (samplesStatus == SAMPLES_OUTDATED)
            pthread_cond_wait(&samplesStatusCondition, &samplesMutex);

        if (samplesStatus == SAMPLES_QUIT) {
            pthread_mutex_unlock(&samplesMutex);
            break;
        }

        bool disconnect = samplesStatus == SAMPLES_READY_WITH_DISCONNECT;

        if (firstSample) {
            // Write header
            StrBuf_addf(&sb, "# ===========================================\n");
            StrBuf_addf(&sb, "# timeslice=%u, slack=%u, packetLoss=%d%s\n",
                numSamples, slack, packetLoss,
                streamMode ? " STREAM" : "");
            StrBuf_addf(&sb, "#acc1x,acc1y,acc1z,"
                "gyro1x,gyro1y,gyro1z,"
                "acc2x,acc2y,acc2z,"
                "gyro2x,gyro2y,gyro2z\n");
        }

        if (streamMode && disconnect)
            StrBuf_addf(&sb, "# --- disconnect ---\n");

        for (unsigned i = 0; i < numSamples; i++) {
            StrBuf_addf(&sb,
                    "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
                    samples[i].acc1.x,
                    samples[i].acc1.y,
                    samples[i].acc1.z,
                    samples[i].gyro1.x,
                    samples[i].gyro1.y,
                    samples[i].gyro1.z,
                    samples[i].acc2.x,
                    samples[i].acc2.y,
                    samples[i].acc2.z,
                    samples[i].gyro2.x,
                    samples[i].gyro2.y,
                    samples[i].gyro2.z);
        }

        if (!streamMode)
            StrBuf_addf(&sb, "# ---\n");

        samplesStatus = SAMPLES_OUTDATED;
        pthread_mutex_unlock(&samplesMutex);

        if (write_full(fd, sb.buf, sb.len) < 0) {
            if (errno == EPIPE) {
                StrBuf_reset(&sb);
                close(fd);
                goto reset;
            } else {
                die("write to %s failed: %s", mlPipe, strerror(errno));
            }
        }

        StrBuf_reset(&sb);
        firstSample = false;
    }

    StrBuf_release(&sb);
    close(fd);
    return NULL;
}

static pthread_mutex_t powMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t powStatusCondition = PTHREAD_COND_INITIALIZER;
static enum {
    POW_OUTDATED,
    POW_READY,
    POW_READY_WITH_DISCONNECT,
    POW_QUIT
} powStatus = POW_OUTDATED;
static ucomm_Pow poww;
char powPipe[512] = "/dev/stdout";

static void *
powPrinter(void *arg)
{
    StrBuf sb = STRBUF_INIT;
    int fd;

reset:
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    fd = open(powPipe, O_CREAT | O_APPEND | O_WRONLY, 0666);
    if (fd < 0)
        die("failed to open %s: %s", powPipe, strerror(errno));
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    for (;;) {
        // Wait for power measurement to be ready
        pthread_mutex_lock(&powMutex);
        if (powStatus == POW_OUTDATED)
            pthread_cond_wait(&powStatusCondition, &powMutex);

        if (powStatus == POW_QUIT) {
            pthread_mutex_unlock(&powMutex);
            break;
        }

        bool disconnect = powStatus == POW_READY_WITH_DISCONNECT;

        if (disconnect)
            StrBuf_addf(&sb, "# --- disconnect ---\n");

        StrBuf_addf(&sb, "%u, %u\n", poww.voltage, poww.current);

        powStatus = POW_OUTDATED;
        pthread_mutex_unlock(&powMutex);

        if (write_full(fd, sb.buf, sb.len) < 0) {
            if (errno == EPIPE) {
                StrBuf_reset(&sb);
                close(fd);
                goto reset;
            } else {
                die("write to %s failed: %s", powPipe, strerror(errno));
            }
        }

        StrBuf_reset(&sb);
    }

    StrBuf_release(&sb);
    close(fd);
    return NULL;
}

static char help[] =
    "raspi-uart [-h] [-r] [-t TIMEFRAME] [-s SLACK] [-m MLPIPE]\n"
    "\n"
    " -r         Enable stream mode.\n"
    "\n";

int
main(int argc, char *argv[])
{
    int c, ret;
    char *endptr;

    while ((c = getopt(argc, argv, "ht:s:m:l:rp:")) != -1) {
        switch (c) {
        case 'h':
            fputs(help, stdout);
            return 0;
        case 't':
            errno = 0;
            numSamples = strtoul(optarg, &endptr, 10);
            if (errno || endptr == optarg)
                die("-t: invalid value: %s", optarg);
            break;
        case 's':
            errno = 0;
            slack = strtoul(optarg, &endptr, 10);
            if (errno || endptr == optarg)
                die("-s: invalid value: %s", optarg);
            break;
        case 'm':
            strncpy(mlPipe, optarg, sizeof(mlPipe));
            mlPipe[sizeof(mlPipe)-1] = '\0';
            break;
        case 'l':
            errno = 0;
            packetLoss = strtoul(optarg, &endptr, 10);
            if (errno || endptr == optarg)
                die("-l: invalid value: %s", optarg);
            break;
        case 'r':
            streamMode = true;
            break;
        case 'p':
            strncpy(powPipe, optarg, sizeof(powPipe));
            powPipe[sizeof(powPipe)-1] = '\0';
            break;
        case '?':
            return 0;
        default:
            die("BUG: unexpected c value %d", c);
        }
    }

    // These parameters are forced in stream mode.
    if (streamMode) {
        numSamples = 1;
        slack = 1;
    }

    samples = malloc(sizeof(ucomm_Sample) * numSamples);
    if (!samples)
        die("out of memory");

    // Init random
    srand(time(NULL));

    // Init sample assembler
    ucomm_SampleAssembler sampleAssembler;
    ucomm_SampleAssembler_init(&sampleAssembler, numSamples, slack);
    if (streamMode)
        sampleAssembler.ucomm_write = NULL; // NACK sending disabled in stream mode

    // Init pow reader
    ucomm_PowReader powReader;
    ucomm_PowReader_init(&powReader);

    // Open UART
    ucomm_initRaspi();

    // Setup signal handler
    struct sigaction action = {};
    action.sa_handler = onTerm;
    sigaction(SIGINT, &action, NULL);
    signal(SIGPIPE, SIG_IGN);

    pthread_t samplePrinterThread;
    ret = pthread_create(&samplePrinterThread, NULL, samplePrinter, NULL);
    if (ret)
        die("pthread_create failed: %s", strerror(errno));

    pthread_t powPrinterThread;
    ret = pthread_create(&powPrinterThread, NULL, powPrinter, NULL);
    if (ret)
        die("pthread_create failed: %s", strerror(errno));

    bool disconnect = false;
    bool powDisconnect = false;
    unsigned int prevSamplesDropped = sampleAssembler.numSamplesDropped;
    while (!done) {
        ucomm_Message msg;

        ucomm_read(&msg);
        if (packetLoss != 0 && rand() % 100 < packetLoss)
            continue; // simulate packet loss

        bool sampleAssemblerFed = ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
        bool powReaderFed = ucomm_PowReader_feed(&powReader, &msg);
        if (!sampleAssemblerFed && !powReaderFed) {
            info("received unknown message with type %u", msg.header.type);
            continue;
        }

        if (sampleAssemblerFed && sampleAssembler.disconnect)
            disconnect = true;
        if (sampleAssemblerFed && sampleAssembler.numSamplesDropped != prevSamplesDropped)
            disconnect = true;
        prevSamplesDropped = sampleAssembler.numSamplesDropped;

        if (sampleAssemblerFed && sampleAssembler.ready) {
            if (!pthread_mutex_trylock(&samplesMutex)) {
                // Copy samples to shared memory area
                for (unsigned i = sampleAssembler.start, n = 0; n < sampleAssembler.windowSize; n++) {
                    samples[n] = sampleAssembler.sample[i];
                    i = (i + 1) % sampleAssembler.alloc;
                }

                // Ready for samplePrinter thread to read it.
                samplesStatus = disconnect ? SAMPLES_READY_WITH_DISCONNECT : SAMPLES_READY;
                pthread_cond_signal(&samplesStatusCondition);
                pthread_mutex_unlock(&samplesMutex);
                disconnect = false;
            } else {
                disconnect = true;
            }
        }

        if (powReaderFed && powReader.ready) {
            if (!pthread_mutex_trylock(&powMutex)) {
                // Copy pow sample to shared memory area
                poww = powReader.pow;
                // Ready for powPrinter thread to read it.
                powStatus = powDisconnect ? POW_READY_WITH_DISCONNECT : POW_READY;
                pthread_cond_signal(&powStatusCondition);
                pthread_mutex_unlock(&powMutex);
                powDisconnect = false;
            } else {
                powDisconnect = true;
            }
        }
    }

    // Notify samplePrinter to exit
    pthread_mutex_lock(&samplesMutex);
    samplesStatus = SAMPLES_QUIT;
    pthread_cond_signal(&samplesStatusCondition);
    pthread_mutex_unlock(&samplesMutex);
    if (pthread_cancel(samplePrinterThread) < 0)
        die("failed to cancel samplePrinterThread: %s", strerror(errno));

    // Notify powPrinter to exit
    pthread_mutex_lock(&powMutex);
    powStatus = POW_QUIT;
    pthread_cond_signal(&powStatusCondition);
    pthread_mutex_unlock(&powMutex);
    if (pthread_cancel(powPrinterThread) < 0)
        die("failed to cancel powPrinterThread: %s", strerror(errno));

    // Wait for samplePrinter to exit
    pthread_join(samplePrinterThread, NULL);

    // Wait for powReader to exit
    pthread_join(powPrinterThread, NULL);

    // Print statistics
    info("Number of recovered packets: %u", sampleAssembler.numPacketsRecovered);
    info("Number of dropped samples: %u", sampleAssembler.numSamplesDropped);

    return 0;
}
