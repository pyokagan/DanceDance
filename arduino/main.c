#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

static void taskBlink(void *);

void setup() {
    xTaskCreate(taskBlink, "Blink", 128, NULL, 2, NULL);
}

void loop() {
    // do nothing
}

void taskBlink(void *pvParameters) {
    pinMode(LED_BUILTIN, OUTPUT);

    for (;;) {
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
