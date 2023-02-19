#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <queue.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "string.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "message.pb.h"

static QueueHandle_t xQueue = NULL;

#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
#define UART_TX_PIN 4 // pin 6
#define UART_RX_PIN 5 // pin 7

void adc_task(void *pvParameters)
{   
    stdio_init_all();
    
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    const uint KNOB_PIN = 26;

    uint uIValueToSend = 0;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    adc_init();
    adc_gpio_init(KNOB_PIN);
    adc_select_input(0);
    
    while (true) {
        gpio_put(LED_PIN, 1);
        uIValueToSend = adc_read();
        xQueueSend(xQueue, &uIValueToSend, 0U);
        vTaskDelay(10);

        gpio_put(LED_PIN, 0);
        uIValueToSend = adc_read();
        xQueueSend(xQueue, &uIValueToSend, 0U);
        vTaskDelay(10);
    }
}

void telemetry_task(void *pvParameters)
{
    // Init static vars for comms
    uint uiRecievedValue;
    char buffer[] = "000000";
    
    // Init UART
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    while(1){
        xQueueReceive(xQueue, &uiRecievedValue, portMAX_DELAY);
        snprintf(buffer, 5, "%u", uiRecievedValue);
        printf("%s %u \n", "uiRecievedValue =", uiRecievedValue); // this would utilize built-in console
        strcat(buffer, "\r\n");
        uart_puts(UART_ID, buffer);
        // uart_write_blocking()
    }
}

int main()
{
    stdio_init_all();

    xQueue = xQueueCreate(1, sizeof(uint));

    xTaskCreate(adc_task, "ADC_Task", 256, NULL, 1, NULL);
    xTaskCreate(telemetry_task, "telemetry_Task", 256, NULL, 1, NULL);
    
    vTaskStartScheduler();

    while(1){};
}
