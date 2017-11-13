#ifndef _UCOMM_SAMPLEASSEMBLER_H
#define _UCOMM_SAMPLEASSEMBLER_H
#include "ucomm.h"

#ifndef ARDUINO
#include <string.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ucomm_SampleAssembler {
    unsigned int windowSize;
    unsigned int alloc;
    unsigned int start, end;
    ucomm_Sample *sample; // The (partial) sample data
    unsigned int *state; // The state of each sample
    unsigned int numReady; // Number of samples that are ready
    unsigned int numPacketsRecovered; // Number of packets recovered through NACKs.
    unsigned int numSamplesDropped; // Number of samples dropped.
    struct timespec lastTime; // Time when last packet was received.
    bool firstPacket;
    bool disconnect;
    bool ready;
    bool mpu1Disconnected;
    bool mpu2Disconnected;
    void (*ucomm_write)(const ucomm_Message *);
} ucomm_SampleAssembler;

void ucomm_SampleAssembler_init(ucomm_SampleAssembler *,
        unsigned int windowSize, unsigned int slack);

void ucomm_SampleAssembler_release(ucomm_SampleAssembler *);

void ucomm_SampleAssembler_reset(ucomm_SampleAssembler *);

bool ucomm_SampleAssembler_feed(ucomm_SampleAssembler *, const ucomm_Message *);

void ucomm_SampleAssembler_print(const ucomm_SampleAssembler *);

#ifdef __cplusplus
}
#endif

#endif // ARDUINO
#endif // _UCOMM_SAMPLEASSEMBLER_H
