#include "uart1.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <HardwareSerial.h>
#include <HardwareSerial_private.h>
#include <wiring_private.h>

// static HardwareSerial UART1(&UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0);
static HardwareSerial UART1(&UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UCSR1C, &UDR1);
static SemaphoreHandle_t lock_read;

#if defined(UART1_RX_vect)
ISR(UART1_RX_vect)
#elif defined(USART1_RX_vect)
ISR(USART1_RX_vect)
#else
#error "Don't know what the Data Register Empty vector is called for Serial1"
#endif
{
    static BaseType_t yieldWhenComplete;
    yieldWhenComplete = pdFALSE;

    UART1._rx_complete_irq();
    xSemaphoreGiveFromISR(lock_read, &yieldWhenComplete);
    if (yieldWhenComplete)
        taskYIELD();
}

#if defined(UART1_UDRE_vect)
ISR(UART1_UDRE_vect)
#elif defined(USART1_UDRE_vect)
ISR(USART1_UDRE_vect)
#else
#error "Don't know what the Data Register Empty vector is called for Serial1"
#endif
{
    UART1._tx_udr_empty_irq();
}

void uart1_init(void)
{
    lock_read = xSemaphoreCreateBinary();
    UART1.begin(115200);
}

void uart1_write(uint8_t byte)
{
    taskENTER_CRITICAL();
    UART1.write(byte);
    taskEXIT_CRITICAL();
}

uint8_t uart1_read(void)
{
    while (UART1.available() <= 0) {
        while (xSemaphoreTake(lock_read, portMAX_DELAY) != pdTRUE);
    }
    return UART1.read();
}
