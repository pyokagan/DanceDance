#ifndef ARDUINO
#include "ucomm_SampleAssembler.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/**
 * A conservative estimate on how many samples the Arduino will be able to
 * store for retransmission.
 * At the time of writing, the sample size is 13 bytes, so total size of the
 * buffer will be 200 * 13 = 2.6KB.
 */
#define ARDUINO_MAX_BUFFER 200

#define STATE_ACC1 0x01
#define STATE_ACC2 0x02

#define STATE_READY (STATE_ACC1 | STATE_ACC2)

void
ucomm_SampleAssembler_init(ucomm_SampleAssembler *self,
        unsigned int windowSize, unsigned int slack)
{
    self->windowSize = windowSize;
    self->alloc = windowSize + slack + 1;
    self->start = 0;
    self->end = 0;
    self->sample = malloc(self->alloc * sizeof(*self->sample));
    self->state = malloc(self->alloc * sizeof(*self->state));
    self->disconnect = false;
    self->ready = false;
    self->numReady = 0;
}

void
ucomm_SampleAssembler_release(ucomm_SampleAssembler *self)
{
    free(self->sample);
    free(self->state);
    self->alloc = 0;
    self->sample = NULL;
    self->state = NULL;
}

void
ucomm_SampleAssembler_reset(ucomm_SampleAssembler *self)
{
    self->disconnect = true;
    self->ready = false;
    self->start = 0;
    self->end = 0;
    self->numReady = 0;
}

static bool
isSampleType(ucomm_type_t type)
{
    return type == UCOMM_MESSAGE_ACC1 ||
        type == UCOMM_MESSAGE_ACC2;
}

static bool
isEmpty(const ucomm_SampleAssembler *self)
{
    return self->start == self->end;
}

static bool
isFull(const ucomm_SampleAssembler *self)
{
    return (self->end + 1) % self->alloc == self->start;
}

static unsigned int
getLastIdx(const ucomm_SampleAssembler *self)
{
    return !self->end ? self->alloc - 1 : self->end - 1;
}

static unsigned int
getLen(const ucomm_SampleAssembler *self)
{
    if (self->end < self->start)
        return self->end + (self->alloc - self->start);
    else
        return self->end - self->start;
}

static bool
getIdx(const ucomm_SampleAssembler *self, ucomm_id_t id, unsigned int *idx)
{
    if (isEmpty(self))
        return false;

    ucomm_id_t endId = self->sample[getLastIdx(self)].id;

    // Assumption: id <= endId
    ucomm_id_t backOffset = id > endId ? ((ucomm_id_t)-1) - id + endId + 1 : endId - id;
    if (backOffset >= ARDUINO_MAX_BUFFER || backOffset > getLen(self) - 1)
        return false;

    if (idx) {
        if (getLastIdx(self) >= backOffset)
            *idx = getLastIdx(self) - backOffset;
        else
            *idx = self->alloc - (backOffset - getLastIdx(self));
    }

    return true;
}

/**
 * Drops the oldest sample.
 */
static void
dropOne(ucomm_SampleAssembler *self)
{
    if (isEmpty(self))
        return; // we are empty, nothing to drop.

    if (self->state[self->start] == STATE_READY)
        self->numReady--;

    if (getLen(self) > self->windowSize) {
        unsigned int incomingIdx = (self->start + self->windowSize) % self->alloc;
        if (self->state[incomingIdx] == STATE_READY)
            self->numReady++;
    }

    self->start = (self->start + 1) % self->alloc;
}

/**
 * Allocates a new empty sample and returns its idx
 */
static unsigned int
allocOne(ucomm_SampleAssembler *self, ucomm_id_t id)
{
    if (isFull(self))
        dropOne(self);

    self->state[self->end] = 0;
    self->sample[self->end].id = id;
    unsigned int idx = self->end;
    self->end = (self->end + 1) % self->alloc;
    return idx;
}

bool
ucomm_SampleAssembler_feed(ucomm_SampleAssembler *self, const ucomm_Message *msg)
{
    unsigned int idx;

    if (!isSampleType(msg->header.type))
        return false;

    self->disconnect = false;

    if (self->ready) {
        for (unsigned int i = 0; i < self->windowSize; i++)
            dropOne(self);
        self->ready = false;
    }

    // Create empty slots if this is a new message
    if (!getIdx(self, msg->header.id, &idx)) {
        if (!isEmpty(self)) {
            ucomm_id_t id;
            for (id = self->sample[self->start].id + getLen(self); id != msg->header.id; id++)
                allocOne(self, id);
        }
        idx = allocOne(self, msg->header.id);
    }

    unsigned int prevState = self->state[idx];

    switch (msg->header.type) {
    case UCOMM_MESSAGE_ACC1:
        self->sample[idx].acc1.x = msg->acc.x;
        self->sample[idx].acc1.y = msg->acc.y;
        self->sample[idx].acc1.z = msg->acc.z;
        self->state[idx] |= STATE_ACC1;
        break;
    case UCOMM_MESSAGE_ACC2:
        self->sample[idx].acc2.x = msg->acc.x;
        self->sample[idx].acc2.y = msg->acc.y;
        self->sample[idx].acc2.z = msg->acc.z;
        self->state[idx] |= STATE_ACC2;
        break;
    default:
        // should not occur: isSampleType() should have taken care of this
        break;
    }

    unsigned int offset = idx >= self->start ? idx - self->start : idx + (self->alloc - self->start);
    if (prevState != STATE_READY && self->state[idx] == STATE_READY && offset < self->windowSize) {
        self->numReady++;
    }

    if (self->numReady >= self->windowSize)
        self->ready = true;

    return true;
}

void
ucomm_SampleAssembler_print(const ucomm_SampleAssembler *self)
{
    printf("--- start=%u end=%u numReady=%u ready=%d\n", self->start, self->end, self->numReady, self->ready);
    for (unsigned int i = self->start; i != self->end; i = (i + 1) % self->alloc) {
        printf("%u, id=%u, state=%u, acc1x=%d, acc1y=%d, acc1z=%d, acc2x=%d, acc2y=%d, acc2z=%d\n",
                i, self->sample[i].id, self->state[i], self->sample[i].acc1.x, self->sample[i].acc1.y,
                self->sample[i].acc1.z, self->sample[i].acc2.x, self->sample[i].acc2.y, self->sample[i].acc2.z);
    }
    puts("---");
}

#endif // ARDUINO
