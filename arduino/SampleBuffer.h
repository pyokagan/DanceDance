#ifndef _ARDUINO_SAMPLEBUFFER_H_
#define _ARDUINO_SAMPLEBUFFER_H_
#include <ucomm.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Number of samples to store for retransmission, plus one.
// (Max: 255)
#define SAMPLE_BUFFER_SIZE 100

typedef struct SampleBuffer {
    ucomm_Sample sample[SAMPLE_BUFFER_SIZE];
    uint8_t start, end;
} SampleBuffer;

void SampleBuffer_init(SampleBuffer *);

/**
 * Returns the idx of the last element.
 */
uint8_t SampleBuffer_getLastIdx(const SampleBuffer *);

uint8_t SampleBuffer_getLen(const SampleBuffer *);

/**
 * If the sample with `id` exists in the buffer, sets its index as `idx` and returns true.
 * Otherwise, returns false.
 */
bool SampleBuffer_getSampleIdx(const SampleBuffer *, ucomm_id_t id, uint8_t *idx);

/**
 * Drops the oldest sample.
 */
void SampleBuffer_dropOne(SampleBuffer *);

/**
 * Puts a new sample.
 * If the buffer is full, the oldest sample will be dropped.
 *
 * Returns its idx.
 */
uint8_t SampleBuffer_put(SampleBuffer *, const ucomm_Sample *);

#ifdef __cplusplus
}
#endif

#endif // _ARDUINO_SAMPLEBUFFER_H_
