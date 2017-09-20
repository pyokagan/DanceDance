#include "taskComm.h"
#include "taskRecv.h"
#include "taskI2C.h"
#include "taskPow.h"

void uart1_ISR() {
	if (!RECV_FLAG) {
		RECV_FLAG = 1;
		xSemaphoreGiveFromISR(lock, NULL);
	}
}

void setup() {
    Serial.begin(115200); // serial for debug messages
	attachInterrupt(0, uart1_ISR, RISING);
    taskComm_setup();
    taskI2C_setup();
    xTaskCreate(taskComm, "Comm", 128, NULL, 1, NULL);
    xTaskCreate(taskRecv, "Recv", 128, NULL, 1, NULL);
    xTaskCreate(taskPow, "Pow", 128, NULL, 2, NULL);
    xTaskCreate(taskI2C, "I2C", 128, NULL, 3, NULL);
}

void loop() {
}
