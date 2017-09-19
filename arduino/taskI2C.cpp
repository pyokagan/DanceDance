#include "taskI2C.h"
#include "taskComm.h"
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <stdbool.h>

static MPU6050 mpu1(0x68);
static MPU6050 mpu2(0x69);

static bool mpu1Active, mpu2Active;

void
taskI2C_setup()
{
    Wire.begin();
    mpu1.initialize();
    mpu2.initialize();

    mpu1Active = mpu1.testConnection();
    mpu2Active = mpu2.testConnection();

    if (mpu1Active)
        mpu1.setDLPFMode(4);

    if (mpu2Active)
        mpu2.setDLPFMode(4);
}

void
taskI2C(void *pvParameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();
    taskComm_Command cmd;
    cmd.type = TASKCOMM_COMMAND_SEND_SAMPLE;
    cmd.sendSample.sample.acc1.x = 0;
    cmd.sendSample.sample.acc1.y = 0;
    cmd.sendSample.sample.acc1.z = 0;
    cmd.sendSample.sample.gyro1.x = 0;
    cmd.sendSample.sample.gyro1.y = 0;
    cmd.sendSample.sample.gyro1.z = 0;
    cmd.sendSample.sample.acc2.x = 0;
    cmd.sendSample.sample.acc2.y = 0;
    cmd.sendSample.sample.acc2.z = 0;
    cmd.sendSample.sample.gyro2.x = 0;
    cmd.sendSample.sample.gyro2.y = 0;
    cmd.sendSample.sample.gyro2.z = 0;

    for (;;) {
        if (mpu1Active)
            mpu1.getMotion6(&cmd.sendSample.sample.acc1.x,
                    &cmd.sendSample.sample.acc1.y,
                    &cmd.sendSample.sample.acc1.z,
                    &cmd.sendSample.sample.gyro1.x,
                    &cmd.sendSample.sample.gyro1.y,
                    &cmd.sendSample.sample.gyro1.z);

        if (mpu2Active)
            mpu2.getMotion6(&cmd.sendSample.sample.acc2.x,
                    &cmd.sendSample.sample.acc2.y,
                    &cmd.sendSample.sample.acc2.z,
                    &cmd.sendSample.sample.gyro2.x,
                    &cmd.sendSample.sample.gyro2.y,
                    &cmd.sendSample.sample.gyro2.z);

        while (!xQueueSendToBack(taskComm_queue, &cmd, portMAX_DELAY));

        vTaskDelayUntil(&lastWakeTime, 23 / portTICK_PERIOD_MS);
    }
}
