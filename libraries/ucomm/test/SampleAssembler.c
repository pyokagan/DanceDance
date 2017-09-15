#include "cheat.h"
#include "cheats.h"
#include "../src/ucomm.h"
#include "../src/ucomm_SampleAssembler.h"

CHEAT_TEST(works,
    ucomm_SampleAssembler sampleAssembler;
    ucomm_Message msg;

    ucomm_SampleAssembler_init(&sampleAssembler, 2, 1);
    sampleAssembler.start = sampleAssembler.end = 2;

    cheat_assert(sampleAssembler.numReady == 0);
    cheat_assert(sampleAssembler.alloc == 4);

    msg.header.id = 255;
    msg.header.type = UCOMM_MESSAGE_ACC1;
    msg.acc.x = 1;
    msg.acc.y = 2;
    msg.acc.z = 3;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    cheat_assert_unsigned_int(sampleAssembler.start, 2);
    cheat_assert_unsigned_int(sampleAssembler.end, 3);
    cheat_assert_unsigned_int(sampleAssembler.numReady, 0);
    cheat_assert_unsigned_int(sampleAssembler.sample[2].id, 255);
    cheat_assert(sampleAssembler.sample[2].acc1.x == 1);
    cheat_assert(sampleAssembler.sample[2].acc1.y == 2);
    cheat_assert(sampleAssembler.sample[2].acc1.z == 3);
    cheat_assert(sampleAssembler.state[2] == 0x01);

    // A new sample appears without the previous sample being fully completed
    msg.header.id = 0;
    msg.header.type = UCOMM_MESSAGE_ACC1;
    msg.acc.x = 4;
    msg.acc.y = 5;
    msg.acc.z = 6;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    cheat_assert_unsigned_int(sampleAssembler.start, 2);
    cheat_assert_unsigned_int(sampleAssembler.end, 0);
    cheat_assert_unsigned_int(sampleAssembler.numReady, 0);
    cheat_assert_unsigned_int(sampleAssembler.sample[3].id, 0);
    cheat_assert_int(sampleAssembler.sample[3].acc1.x, 4);
    cheat_assert_int(sampleAssembler.sample[3].acc1.y, 5);
    cheat_assert_int(sampleAssembler.sample[3].acc1.z, 6);
    cheat_assert_unsigned_int(sampleAssembler.state[3], 0x01);

    // Another new sample appears without the previous sample being fully
    // completed
    msg.header.id = 1;
    msg.header.type = UCOMM_MESSAGE_ACC1;
    msg.acc.x = 7;
    msg.acc.y = 8;
    msg.acc.z = 9;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    cheat_assert_unsigned_int(sampleAssembler.start, 2);
    cheat_assert_unsigned_int(sampleAssembler.end, 1);
    cheat_assert_unsigned_int(sampleAssembler.numReady, 0);
    cheat_assert_unsigned_int(sampleAssembler.sample[0].id, 1);
    cheat_assert_int(sampleAssembler.sample[0].acc1.x, 7);
    cheat_assert_int(sampleAssembler.sample[0].acc1.y, 8);
    cheat_assert_int(sampleAssembler.sample[0].acc1.z, 9);
    cheat_assert_unsigned_int(sampleAssembler.state[3], 0x01);

    // Complete sample 0
    msg.header.id = 0;
    msg.header.type = UCOMM_MESSAGE_ACC2;
    msg.acc.x = 10;
    msg.acc.y = 11;
    msg.acc.z = 12;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    cheat_assert_unsigned_int(sampleAssembler.numReady, 1);
    cheat_assert_int(sampleAssembler.sample[3].acc1.x, 4);
    cheat_assert_int(sampleAssembler.sample[3].acc1.y, 5);
    cheat_assert_int(sampleAssembler.sample[3].acc1.z, 6);
    cheat_assert_int(sampleAssembler.sample[3].acc2.x, 10);
    cheat_assert_int(sampleAssembler.sample[3].acc2.y, 11);
    cheat_assert_int(sampleAssembler.sample[3].acc2.z, 12);

    // Complete sample 1
    // numReady should not change since the sample is within the "slack" area
    msg.header.id = 1;
    msg.header.type = UCOMM_MESSAGE_ACC2;
    msg.acc.x = 13;
    msg.acc.y = 14;
    msg.acc.z = 15;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    cheat_assert_unsigned_int(sampleAssembler.numReady, 1);
    cheat_assert_int(sampleAssembler.sample[0].acc1.x, 7);
    cheat_assert_int(sampleAssembler.sample[0].acc1.y, 8);
    cheat_assert_int(sampleAssembler.sample[0].acc1.z, 9);
    cheat_assert_int(sampleAssembler.sample[0].acc2.x, 13);
    cheat_assert_int(sampleAssembler.sample[0].acc2.y, 14);
    cheat_assert_int(sampleAssembler.sample[0].acc2.z, 15);

    // Complete sample 255
    // The ready flag should now be asserted since we successfully built a
    // contiguous block of 2 samples (255 and 0)
    msg.header.id = 255;
    msg.header.type = UCOMM_MESSAGE_ACC2;
    msg.acc.x = 16;
    msg.acc.y = 17;
    msg.acc.z = 18;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    cheat_assert_unsigned_int(sampleAssembler.numReady, 2);
    cheat_assert(sampleAssembler.ready);
    cheat_assert(sampleAssembler.sample[2].acc1.x == 1);
    cheat_assert(sampleAssembler.sample[2].acc1.y == 2);
    cheat_assert(sampleAssembler.sample[2].acc1.z == 3);
    cheat_assert_int(sampleAssembler.sample[2].acc2.x, 16);
    cheat_assert_int(sampleAssembler.sample[2].acc2.y, 17);
    cheat_assert_int(sampleAssembler.sample[2].acc2.z, 18);

    // New sample
    msg.header.id = 3;
    msg.header.type = UCOMM_MESSAGE_ACC2;
    msg.acc.x = 19;
    msg.acc.y = 20;
    msg.acc.z = 21;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    cheat_assert_unsigned_int(sampleAssembler.numReady, 1);
    cheat_assert_unsigned_int(sampleAssembler.start, 0);
    cheat_assert_unsigned_int(sampleAssembler.end, 3);
    cheat_assert(!sampleAssembler.ready);

    // Sample with ID=2 was implicitly created
    cheat_assert_int(sampleAssembler.sample[1].id, 2);
    cheat_assert_unsigned_int(sampleAssembler.state[1], 0);

    cheat_assert_int(sampleAssembler.sample[2].id, 3);
    cheat_assert_unsigned_int(sampleAssembler.state[2], 0x02);
    cheat_assert_int(sampleAssembler.sample[2].acc2.x, 19);
    cheat_assert_int(sampleAssembler.sample[2].acc2.y, 20);
    cheat_assert_int(sampleAssembler.sample[2].acc2.z, 21);
)

// Test for a large window that is larger than the range of ucomm_id_t
CHEAT_TEST(large_window,
    ucomm_SampleAssembler sampleAssembler;
    ucomm_Message msg;

    ucomm_SampleAssembler_init(&sampleAssembler, 500, 1);

    // Send in 500 proper samples
    for (unsigned int i = 0; i < 500; i++) {
        msg.header.id = i;
        msg.header.type = UCOMM_MESSAGE_ACC1;
        msg.acc.x = 1;
        msg.acc.y = 2;
        msg.acc.z = 3;
        ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

        msg.header.id = i;
        msg.header.type = UCOMM_MESSAGE_ACC2;
        ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
    }

    cheat_assert(sampleAssembler.ready);
)
