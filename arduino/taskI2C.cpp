#include "taskI2C.h"
#include "taskComm.h"
#include <I2Cdev.h>
#include <MPU6050.h>
#include <stdbool.h>
#include <semphr.h>
#include <TimerOne.h>

static MPU6050 mpu1(0x68);
static MPU6050 mpu2(0x69);

static bool mpu1Active, mpu2Active;
static SemaphoreHandle_t timerLock;

static void
timerIsr(void)
{
    static BaseType_t yieldWhenComplete;
    yieldWhenComplete = pdFALSE;

    xSemaphoreGiveFromISR(timerLock, &yieldWhenComplete);
    if (yieldWhenComplete)
        taskYIELD();
}

static bool
setupMpu(MPU6050* mpu)
{
    if (!mpu->initialize())
        return false;
    if (!mpu->setDLPFMode(4))
        return false;
    return true;
}

static bool
isMpu1Zeroes(const ucomm_Sample *sample)
{
    return (sample->acc1.x == 0 && sample->acc1.y == 0 && sample->acc1.z == 0) ||
        (sample->gyro1.x == 0 && sample->gyro1.y == 0 && sample->gyro1.z == 0);
}

static bool
isMpu2Zeroes(const ucomm_Sample *sample)
{
    return (sample->acc2.x == 0 && sample->acc2.y == 0 && sample->acc2.z == 0) ||
        (sample->gyro2.x == 0 && sample->gyro2.y == 0 && sample->gyro2.z == 0);
}

void
taskI2C_setup()
{
    Fastwire::setup(100, true);

    mpu1Active = setupMpu(&mpu1);
    mpu2Active = setupMpu(&mpu2);

    timerLock = xSemaphoreCreateBinary();
    Timer1.initialize(23L * 1000L);
    Timer1.attachInterrupt(timerIsr, 0);
}

void
taskI2C(void *pvParameters)
{
    taskComm_Command cmd = {};
    cmd.type = TASKCOMM_COMMAND_SEND_SAMPLE;

    for (;;) {
        if (mpu1Active && !mpu1.getMotion6(&cmd.sendSample.sample.acc1.x,
                    &cmd.sendSample.sample.acc1.y,
                    &cmd.sendSample.sample.acc1.z,
                    &cmd.sendSample.sample.gyro1.x,
                    &cmd.sendSample.sample.gyro1.y,
                    &cmd.sendSample.sample.gyro1.z))
            mpu1Active = false;

        if (mpu2Active && !mpu2.getMotion6(&cmd.sendSample.sample.acc2.x,
                    &cmd.sendSample.sample.acc2.y,
                    &cmd.sendSample.sample.acc2.z,
                    &cmd.sendSample.sample.gyro2.x,
                    &cmd.sendSample.sample.gyro2.y,
                    &cmd.sendSample.sample.gyro2.z))
            mpu2Active = false;

        if (mpu1Active && isMpu1Zeroes(&cmd.sendSample.sample))
            mpu1Active = false;

        if (mpu2Active && isMpu2Zeroes(&cmd.sendSample.sample))
            mpu2Active = false;

        if (!mpu1Active || !mpu2Active) {
            if (!mpu1Active)
                mpu1Active = setupMpu(&mpu1);
            if (!mpu2Active)
                mpu2Active = setupMpu(&mpu2);
        } else {
            while (xQueueSendToBack(taskComm_queue, &cmd, portMAX_DELAY) != pdTRUE);
        }

        /*mpu1.getMotion6(&cmd.sendSample.sample.acc1.x,
          &cmd.sendSample.sample.acc1.y,
          &cmd.sendSample.sample.acc1.z,
          &cmd.sendSample.sample.gyro1.x,
          &cmd.sendSample.sample.gyro1.y,
          &cmd.sendSample.sample.gyro1.z);

          mpu2.getMotion6(&cmd.sendSample.sample.acc2.x,
          &cmd.sendSample.sample.acc2.y,
          &cmd.sendSample.sample.acc2.z,
          &cmd.sendSample.sample.gyro2.x,
          &cmd.sendSample.sample.gyro2.y,
          &cmd.sendSample.sample.gyro2.z);

          while (!xQueueSendToBack(taskComm_queue, &cmd, portMAX_DELAY));*/

        while (xSemaphoreTake(timerLock, portMAX_DELAY) != pdTRUE);
    }
}
