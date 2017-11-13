#include "cheat.h"
#include "cheats.h"
#include "../src/ucomm.h"
#include "../src/ucomm_SampleAssembler.h"

CHEAT_TEST(works,
    ucomm_SampleAssembler sampleAssembler;
    ucomm_Message msg;

    ucomm_SampleAssembler_init(&sampleAssembler, 2, 1);
    sampleAssembler.start = sampleAssembler.end = 2;
    sampleAssembler.ucomm_write = NULL;

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
    msg.header.type = UCOMM_MESSAGE_ACC2_RESEND;
    msg.acc.x = 10;
    msg.acc.y = 11;
    msg.acc.z = 12;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
    msg.header.type = UCOMM_MESSAGE_GYRO1_RESEND;
    msg.gyro.x = 100;
    msg.gyro.y = 101;
    msg.gyro.z = 102;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
    msg.header.type = UCOMM_MESSAGE_GYRO2_RESEND;
    msg.gyro.x = 103;
    msg.gyro.y = 104;
    msg.gyro.z = 105;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    cheat_assert_unsigned_int(sampleAssembler.numReady, 1);
    cheat_assert_int(sampleAssembler.sample[3].acc1.x, 4);
    cheat_assert_int(sampleAssembler.sample[3].acc1.y, 5);
    cheat_assert_int(sampleAssembler.sample[3].acc1.z, 6);
    cheat_assert_int(sampleAssembler.sample[3].acc2.x, 10);
    cheat_assert_int(sampleAssembler.sample[3].acc2.y, 11);
    cheat_assert_int(sampleAssembler.sample[3].acc2.z, 12);
    cheat_assert_int(sampleAssembler.sample[3].gyro1.x, 100);
    cheat_assert_int(sampleAssembler.sample[3].gyro1.y, 101);
    cheat_assert_int(sampleAssembler.sample[3].gyro1.z, 102);
    cheat_assert_int(sampleAssembler.sample[3].gyro2.x, 103);
    cheat_assert_int(sampleAssembler.sample[3].gyro2.y, 104);
    cheat_assert_int(sampleAssembler.sample[3].gyro2.z, 105);

    // Complete sample 1
    // numReady should not change since the sample is within the "slack" area
    msg.header.id = 1;
    msg.header.type = UCOMM_MESSAGE_ACC2_RESEND;
    msg.acc.x = 13;
    msg.acc.y = 14;
    msg.acc.z = 15;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
    msg.header.type = UCOMM_MESSAGE_GYRO1_RESEND;
    msg.gyro.x = 106;
    msg.gyro.y = 107;
    msg.gyro.z = 108;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
    msg.header.type = UCOMM_MESSAGE_GYRO2_RESEND;
    msg.gyro.x = 109;
    msg.gyro.y = 110;
    msg.gyro.z = 111;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    cheat_assert_unsigned_int(sampleAssembler.numReady, 1);
    cheat_assert_int(sampleAssembler.sample[0].acc1.x, 7);
    cheat_assert_int(sampleAssembler.sample[0].acc1.y, 8);
    cheat_assert_int(sampleAssembler.sample[0].acc1.z, 9);
    cheat_assert_int(sampleAssembler.sample[0].acc2.x, 13);
    cheat_assert_int(sampleAssembler.sample[0].acc2.y, 14);
    cheat_assert_int(sampleAssembler.sample[0].acc2.z, 15);
    cheat_assert_int(sampleAssembler.sample[0].gyro1.x, 106);
    cheat_assert_int(sampleAssembler.sample[0].gyro1.y, 107);
    cheat_assert_int(sampleAssembler.sample[0].gyro1.z, 108);
    cheat_assert_int(sampleAssembler.sample[0].gyro2.x, 109);
    cheat_assert_int(sampleAssembler.sample[0].gyro2.y, 110);
    cheat_assert_int(sampleAssembler.sample[0].gyro2.z, 111);

    // Complete sample 255
    // The ready flag should now be asserted since we successfully built a
    // contiguous block of 2 samples (255 and 0)
    msg.header.id = 255;
    msg.header.type = UCOMM_MESSAGE_ACC2_RESEND;
    msg.acc.x = 16;
    msg.acc.y = 17;
    msg.acc.z = 18;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
    msg.header.type = UCOMM_MESSAGE_GYRO1_RESEND;
    msg.gyro.x = 112;
    msg.gyro.y = 113;
    msg.gyro.z = 114;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
    msg.header.type = UCOMM_MESSAGE_GYRO2_RESEND;
    msg.gyro.x = 115;
    msg.gyro.y = 116;
    msg.gyro.z = 117;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    cheat_assert_unsigned_int(sampleAssembler.numReady, 2);
    cheat_assert(sampleAssembler.ready);
    cheat_assert(sampleAssembler.sample[2].acc1.x == 1);
    cheat_assert(sampleAssembler.sample[2].acc1.y == 2);
    cheat_assert(sampleAssembler.sample[2].acc1.z == 3);
    cheat_assert_int(sampleAssembler.sample[2].acc2.x, 16);
    cheat_assert_int(sampleAssembler.sample[2].acc2.y, 17);
    cheat_assert_int(sampleAssembler.sample[2].acc2.z, 18);
    cheat_assert_int(sampleAssembler.sample[2].gyro1.x, 112);
    cheat_assert_int(sampleAssembler.sample[2].gyro1.y, 113);
    cheat_assert_int(sampleAssembler.sample[2].gyro1.z, 114);
    cheat_assert_int(sampleAssembler.sample[2].gyro2.x, 115);
    cheat_assert_int(sampleAssembler.sample[2].gyro2.y, 116);
    cheat_assert_int(sampleAssembler.sample[2].gyro2.z, 117);

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
    sampleAssembler.ucomm_write = NULL;

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

        msg.header.id = i;
        msg.header.type = UCOMM_MESSAGE_GYRO1;
        msg.gyro.x = 4;
        msg.gyro.y = 5;
        msg.gyro.z = 6;
        ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

        msg.header.id = i;
        msg.header.type = UCOMM_MESSAGE_GYRO2;
        ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
    }

    cheat_assert(sampleAssembler.ready);
)

// Test for mpu disconnect
CHEAT_TEST(mpu_disconnect,
    ucomm_SampleAssembler sampleAssembler;
    ucomm_Message msg;

    ucomm_SampleAssembler_init(&sampleAssembler, 1, 1);
    sampleAssembler.ucomm_write = NULL;

    cheat_assert_unsigned_int(sampleAssembler.numReady, 0);
    cheat_assert_unsigned_int(sampleAssembler.start, 0);
    cheat_assert_unsigned_int(sampleAssembler.end, 0);
    cheat_assert(!sampleAssembler.mpu1Disconnected);
    cheat_assert(!sampleAssembler.mpu2Disconnected);

    msg.header.id = 1;
    msg.header.type = UCOMM_MESSAGE_ACC1;
    msg.acc.x = 0;
    msg.acc.y = 0;
    msg.acc.z = 0;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    msg.header.type = UCOMM_MESSAGE_GYRO1;
    msg.gyro.x = 0;
    msg.gyro.y = 0;
    msg.gyro.z = 0;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    msg.header.type = UCOMM_MESSAGE_ACC2;
    msg.acc.x = 1;
    msg.acc.y = 2;
    msg.acc.z = 3;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    msg.header.type = UCOMM_MESSAGE_ACC1;
    msg.header.id = 2;
    msg.acc.x = 1;
    msg.acc.y = 2;
    msg.acc.z = 3;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
    cheat_assert_unsigned_int(sampleAssembler.start, 0);
    cheat_assert_unsigned_int(sampleAssembler.end, 2);
    cheat_assert_unsigned_int(sampleAssembler.numReady, 0);

    msg.header.type = UCOMM_MESSAGE_GYRO1;
    msg.gyro.x = 1;
    msg.gyro.y = 2;
    msg.gyro.z = 3;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    msg.header.type = UCOMM_MESSAGE_ACC2;
    msg.acc.x = 4;
    msg.acc.y = 5;
    msg.acc.z = 6;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);

    msg.header.type = UCOMM_MESSAGE_GYRO2;
    msg.gyro.x = 4;
    msg.gyro.y = 5;
    msg.gyro.z = 6;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
    cheat_assert_unsigned_int(sampleAssembler.numReady, 0);
    cheat_assert(!sampleAssembler.mpu1Disconnected);
    cheat_assert(!sampleAssembler.mpu2Disconnected);
    cheat_assert(!sampleAssembler.ready);

    msg.header.type = UCOMM_MESSAGE_GYRO2_RESEND;
    msg.header.id = 1;
    msg.gyro.x = 7;
    msg.gyro.y = 8;
    msg.gyro.z = 9;
    ucomm_SampleAssembler_feed(&sampleAssembler, &msg);
    cheat_assert_unsigned_int(sampleAssembler.numReady, 1);
    cheat_assert_unsigned_int(sampleAssembler.start, 1);
    cheat_assert_unsigned_int(sampleAssembler.end, 2);
    cheat_assert(sampleAssembler.ready);
    cheat_assert_unsigned_int(sampleAssembler.numSamplesDropped, 0);
    cheat_assert(sampleAssembler.disconnect);
    cheat_assert(!sampleAssembler.mpu1Disconnected);
    cheat_assert(!sampleAssembler.mpu2Disconnected);
)
