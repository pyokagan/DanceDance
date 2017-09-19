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
#include <ucomm.h>
#include <ucomm_SampleAssembler.h>

__attribute__((noreturn))
static void
die(const char *fmt, ...)
{
    char buf[1024];
    va_list va;

    va_start(va, fmt);
    vsnprintf(buf, sizeof(buf), fmt, va);
    va_end(va);

    fprintf(stderr, "FATAL: %s\n", buf);
    abort();
}

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
static enum {
    SAMPLES_OUTDATED,
    SAMPLES_READY,
    SAMPLES_READY_WITH_DISCONNECT,
    SAMPLES_QUIT
} samplesStatus = SAMPLES_OUTDATED;
static ucomm_Sample *samples;
static ucomm_Sample *samplePrinterSamples;
FILE *samplePrinterFile;

static void *
samplePrinter(void *arg)
{
    bool firstSample = true;

    for (;;) {
        bool quit, disconnect;

        // Wait for timeslice to be ready
        pthread_mutex_lock(&samplesMutex);
        if (samplesStatus == SAMPLES_OUTDATED)
            pthread_cond_wait(&samplesStatusCondition, &samplesMutex);

        quit = samplesStatus == SAMPLES_QUIT;
        disconnect = samplesStatus == SAMPLES_READY_WITH_DISCONNECT;

        // Samples are ready, copy them for our private usage
        memcpy(samplePrinterSamples, samples, sizeof(ucomm_Sample) * numSamples);
        samplesStatus = SAMPLES_OUTDATED;
        pthread_mutex_unlock(&samplesMutex);

        if (quit)
            return NULL;

        if (!firstSample && !streamMode)
            fprintf(samplePrinterFile, "# ---\n");

        if (streamMode && disconnect)
            fprintf(samplePrinterFile, "# --- disconnect ---\n");

        for (unsigned i = 0; i < numSamples; i++) {
            fprintf(samplePrinterFile, "%d, %d, %d, %d, %d, %d\n",
                    samplePrinterSamples[i].acc1.x,
                    samplePrinterSamples[i].acc1.y,
                    samplePrinterSamples[i].acc1.z,
                    samplePrinterSamples[i].acc2.x,
                    samplePrinterSamples[i].acc2.y,
                    samplePrinterSamples[i].acc2.z);
        }
        fflush(samplePrinterFile);

        firstSample = false;
    }

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
    unsigned int slack = 2;
    char mlPipe[512] = "/dev/stdout";
    int c, ret;
    int packetLoss = 0;
    char *endptr;

    while ((c = getopt(argc, argv, "ht:s:m:l:r")) != -1) {
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

    info("mlPipe: %s", mlPipe);

    samples = malloc(sizeof(ucomm_Sample) * numSamples);
    if (!samples)
        die("out of memory");
    samplePrinterSamples = malloc(sizeof(ucomm_Sample) * numSamples);
    if (!samplePrinterSamples)
        die("out of memory");

    // Init random
    srand(time(NULL));

    // Init sample assembler
    ucomm_SampleAssembler sampleAssembler;
    ucomm_SampleAssembler_init(&sampleAssembler, numSamples, slack);
    if (streamMode)
        sampleAssembler.ucomm_write = NULL; // NACK sending disabled in stream mode

    // Open samplePrinterFile
    samplePrinterFile = fopen(mlPipe, "a");
    if (!samplePrinterFile)
        die("failed to open ml pipe: %s", strerror(errno));

    // Open UART
    ucomm_initRaspi();

    // Setup signal handler
    struct sigaction action = {};
    action.sa_handler = onTerm;
    sigaction(SIGINT, &action, NULL);

    // Write header to samplePrinterFile
    fprintf(samplePrinterFile, "# ===========================================\n");
    fprintf(samplePrinterFile, "# timeslice=%u, slack=%u, packetLoss=%d%s\n",
            numSamples, slack, packetLoss,
            streamMode ? " STREAM" : "");
    fprintf(samplePrinterFile, "# acc1x, acc1y, acc1z, acc2x, acc2y, acc2z\n");
    fflush(samplePrinterFile);

    pthread_t samplePrinterThread;
    ret = pthread_create(&samplePrinterThread, NULL, samplePrinter, NULL);
    if (ret)
        die("pthread_create failed: %s", strerror(errno));

    bool disconnect = false;
    unsigned int prevSamplesDropped = sampleAssembler.numSamplesDropped;
    while (!done) {
        ucomm_Message msg;

        ucomm_read(&msg);
        if (packetLoss != 0 && rand() % 100 < packetLoss)
            continue; // simulate packet loss

        if (!ucomm_SampleAssembler_feed(&sampleAssembler, &msg)) {
            info("received unknown message with type %u", msg.header.type);
            continue;
        }

        if (sampleAssembler.disconnect)
            disconnect = true;
        if (sampleAssembler.numSamplesDropped != prevSamplesDropped)
            disconnect = true;
        prevSamplesDropped = sampleAssembler.numSamplesDropped;

        if (sampleAssembler.ready) {
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
    }

    // Notify samplePrinter to exit
    pthread_mutex_lock(&samplesMutex);
    samplesStatus = SAMPLES_QUIT;
    pthread_cond_signal(&samplesStatusCondition);
    pthread_mutex_unlock(&samplesMutex);

    // Wait for samplePrinter to exit
    pthread_join(samplePrinterThread, NULL);

    // Print statistics
    info("Number of recovered packets: %u", sampleAssembler.numPacketsRecovered);
    info("Number of dropped samples: %u", sampleAssembler.numSamplesDropped);

    return 0;
}
