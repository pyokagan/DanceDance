#include <Arduino.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>

static MPU6050 mpu1(0x68);
static MPU6050 mpu2(0x69);

static bool mpu1Active, mpu2Active;

void
setup(void)
{
    Serial.begin(115200);
    Wire.begin();
    mpu1.initialize();
    mpu2.initialize();

    if ((mpu1Active = mpu1.testConnection()))
        Serial.println(F("mpu1 connection successful."));
    else
        Serial.println(F("mpu1 connection failed."));

    if ((mpu2Active = mpu2.testConnection()))
        Serial.println(F("mpu2 connection successful."));
    else
        Serial.println(F("mpu2 connection failed."));

    if (mpu1Active)
        mpu1.setDLPFMode(4); // 21Hz
}

void
loop(void)
{
    int16_t acc1x = 0, acc1y = 0, acc1z = 0;
    int16_t g1x = 0, g1y = 0, g1z = 0;

    int16_t acc2x = 0, acc2y = 0, acc2z = 0;
    int16_t g2x = 0, g2y = 0, g2z = 0;

    if (mpu1Active)
        mpu1.getMotion6(&acc1x, &acc1y, &acc1z, &g1x, &g1y, &g1z);

    if (mpu2Active)
        mpu2.getMotion6(&acc2x, &acc2y, &acc2z, &g2x, &g2y, &g2z);

    if (mpu1Active) {
        Serial.print(acc1x);
        Serial.print(F(", "));
        Serial.print(acc1y);
        Serial.print(F(", "));
        Serial.print(acc1z);
        Serial.print(F(", "));
        Serial.print(g1x);
        Serial.print(F(", "));
        Serial.print(g1y);
        Serial.print(F(", "));
        Serial.print(g1z);
        if (mpu2Active)
            Serial.print(F(", "));
    }

    if (mpu2Active) {
        Serial.print(acc2x);
        Serial.print(F(", "));
        Serial.print(acc2y);
        Serial.print(F(", "));
        Serial.print(acc2z);
        Serial.print(F(", "));
        Serial.print(g2x);
        Serial.print(F(", "));
        Serial.print(g2y);
        Serial.print(F(", "));
        Serial.print(g2z);
    }

    if (mpu2Active || mpu1Active)
        Serial.print(F("\r\n"));

    delay(5);
}
