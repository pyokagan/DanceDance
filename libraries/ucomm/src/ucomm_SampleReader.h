#ifndef _UCOMM_SAMPLEREADER_H
#define _UCOMM_SAMPLEREADER_H
#include <stdbool.h>
#include "ucomm.h"

#ifndef ARDUINO
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ucomm_SampleReader {
    unsigned state; // Internal state
    bool sampleReady; // True if the sample can be read
    bool disconnect;
    bool firstPacket, firstSample;
    ucomm_id_t lastSampleId;
    struct timespec lastTime; // Time when last packet was received, used to detect disconnects
    int numDisconnects;
    int numDropped; // Number of samples lost
    ucomm_Sample sample;
} ucomm_SampleReader;

void ucomm_SampleReader_init(ucomm_SampleReader *);

void ucomm_SampleReader_reset(ucomm_SampleReader *);

bool ucomm_SampleReader_feed(ucomm_SampleReader *, const ucomm_Message *);

#ifdef __cplusplus
}
#endif

#endif // ARDUINO
#endif // _UCOMM_SAMPLEREADER_H
