#include "SampleBuffer.h"

void
SampleBuffer_init(SampleBuffer *self)
{
    self->start = 0;
    self->end = 0;
}

static bool
SampleBuffer_isEmpty(const SampleBuffer *self)
{
    return self->start == self->end;
}

static bool
SampleBuffer_isFull(const SampleBuffer *self)
{
    return (self->end + 1) % SAMPLE_BUFFER_SIZE == self->start;
}

/**
 * Returns the index of the last element.
 */
uint8_t
SampleBuffer_getLastIdx(const SampleBuffer *self)
{
    // assert(!SampleBuffer_isEmpty(self))
    return !self->end ? SAMPLE_BUFFER_SIZE - 1 : self->end - 1;
}

uint8_t
SampleBuffer_getLen(const SampleBuffer *self)
{
    if (self->end < self->start)
        return self->end + (SAMPLE_BUFFER_SIZE - self->start);
    else
        return self->end - self->start;
}

bool
SampleBuffer_getSampleIdx(const SampleBuffer *self, ucomm_id_t id, uint8_t *idx)
{
    if (SampleBuffer_isEmpty(self))
        return false;

    ucomm_id_t lastId = self->sample[SampleBuffer_getLastIdx(self)].id;

    // Assumption: id <= lastId (with wraparound)
    ucomm_id_t backOffset = id > lastId ? ((ucomm_id_t)-1) - id + lastId + 1 : lastId - id;
    if (backOffset > SampleBuffer_getLen(self) - 1)
        return false;

    if (SampleBuffer_getLastIdx(self) >= backOffset)
        *idx = SampleBuffer_getLastIdx(self) - backOffset;
    else
        *idx = SAMPLE_BUFFER_SIZE - (backOffset - SampleBuffer_getLastIdx(self));

    return true;
}

/**
 * Drops the oldest sample.
 */
void
SampleBuffer_dropOne(SampleBuffer *self)
{
    if (SampleBuffer_isEmpty(self))
        return false;

    self->start = (self->start + 1) % SAMPLE_BUFFER_SIZE;
}

/**
 * Puts a new sample.
 */
uint8_t
SampleBuffer_put(SampleBuffer *self, const ucomm_Sample *sample)
{
    if (SampleBuffer_isFull(self))
        SampleBuffer_dropOne(self);

    self->sample[self->end] = *sample;
    uint8_t idx = self->end;
    self->end = (self->end + 1) % SAMPLE_BUFFER_SIZE;
    return idx;
}
