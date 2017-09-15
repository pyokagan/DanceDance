#ifndef ARDUINO
#include "ucomm_SampleReader.h"

#define STATE_ACC1 0
#define STATE_ACC2 1

static void
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

void
ucomm_SampleReader_init(ucomm_SampleReader *self)
{
    self->state = STATE_ACC1;
    self->sampleReady = false;
    self->disconnect = false;
    self->firstPacket = true;
    self->firstSample = true;
    self->numDisconnects = 0;
    self->numDropped = 0;
}

void
ucomm_SampleReader_reset(ucomm_SampleReader *self)
{
    self->state = STATE_ACC1;
    self->sampleReady = false;
    self->firstSample = true;
}

static int
isSampleType(ucomm_type_t type)
{
    return type == UCOMM_MESSAGE_ACC1 ||
        type == UCOMM_MESSAGE_ACC2;
}

bool
ucomm_SampleReader_feed(ucomm_SampleReader *self, const ucomm_Message *msg)
{
    if (!isSampleType(msg->header.type))
        return false; // message not handled

    self->disconnect = false;
    if (!self->firstPacket) {
        struct timespec currentTime, diff;

        clock_gettime(CLOCK_MONOTONIC, &currentTime);
        timespec_diff(&self->lastTime, &currentTime, &diff);
        if (diff.tv_sec >= 1) {
            // A disconnect occurred -- reset connection
            self->disconnect = true;
            self->numDisconnects++;
            ucomm_SampleReader_reset(self);
        }
        self->lastTime = currentTime;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &self->lastTime);
    }
    self->firstPacket = false;

retry:
    self->sampleReady = false;
    switch (self->state) {
    case STATE_ACC1:
        if (msg->header.type == UCOMM_MESSAGE_ACC1) {
            self->sample.id = msg->header.id;
            self->sample.acc1.x = msg->acc.x;
            self->sample.acc1.y = msg->acc.y;
            self->sample.acc1.z = msg->acc.z;
            self->state = STATE_ACC2;
        }
        return true;
    case STATE_ACC2:
        if (msg->header.type != UCOMM_MESSAGE_ACC2 || msg->header.id != self->sample.id) {
            self->state = STATE_ACC1;
            goto retry;
        }
        self->sample.acc2.x = msg->acc.x;
        self->sample.acc2.y = msg->acc.y;
        self->sample.acc2.z = msg->acc.z;
        self->state = STATE_ACC1;
        self->sampleReady = true;

        if (!self->firstSample) {
            // Determine how many samples were dropped, taking into account
            // overflow.
            if (self->sample.id <= self->lastSampleId) {
                // Overflow
                self->numDropped += ((ucomm_id_t)-1) - self->lastSampleId + self->sample.id;
            } else {
                self->numDropped += self->sample.id - self->lastSampleId - 1;
            }
        }
        self->firstSample = false;
        self->lastSampleId = self->sample.id;
        return true;
    default:
        // Invalid state
        self->state = STATE_ACC1;
        return true;
    }
}

#endif
