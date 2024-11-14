#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <queue.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include <pb_encode.h>
#include <pb_decode.h>
#include "nanopb/message.pb.h"

#include "nanocobs/cobs.h"

static QueueHandle_t xQueue = NULL;

#define UART_ID     uart0
#define BAUD_RATE   115200
#define DATA_BITS   8
#define STOP_BITS   1
#define PARITY      UART_PARITY_NONE
#define UART_TX_PIN 0 // pin 6
#define UART_RX_PIN 1 // pin 7

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

// Protobuf encoding function
int prep_buf(uint value, uint8_t* buf, size_t buf_len, size_t* bytes_written)
{
    DataPackage message = DataPackage_init_default;
    message.potentiometer = value;
    message.generator = 3;

    // Encode the message
    pb_ostream_t stream = pb_ostream_from_buffer(buf, buf_len);
    if (!pb_encode(&stream, DataPackage_fields, &message)) {
        // printf("Error: %s\n", PB_GET_ERROR(&stream));
        return 0;
    }
    
    *bytes_written = stream.bytes_written;
    
    return 1;
}

void telemetry_task(void *pvParameters)
{
    // Initialize the message structure
    // DataPackage message = DataPackage_init_default;

    // Init static vars for comms
    uint uiRecievedValue;

    // Allocate buffers to hold the encoded message
    #define bufferA_length  128
    #define bufferB_length  128

    uint8_t bufferA[bufferA_length];
    uint8_t bufferB[bufferB_length];
    size_t bytes_written;
    
    // Init UART
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    while(1){
        xQueueReceive(xQueue, &uiRecievedValue, portMAX_DELAY);
        // printf("%s %u \n", "uiRecievedValue =", uiRecievedValue); // this would utilize built-in console
        
        // Encode message as binary using protobuf
        prep_buf(uiRecievedValue, bufferA, bufferA_length, &bytes_written);

        // Strip zeros
        cobs_ret_t cobs_ret = cobs_encode(bufferA, bytes_written, bufferB, bufferB_length, &bytes_written);

        switch (cobs_ret) {
            case COBS_RET_SUCCESS:
            // printf("cobs encode successful. sending...\n");
            // printf(bufferB);
            uart_write_blocking(UART_ID, bufferB, bytes_written);   // Send the data
            // uart_putc(UART_ID, 0x0);                               // Send termination symbol
            break;

            case COBS_RET_ERR_BAD_ARG:
            // printf("cobs error bad arg\n");
            break;

            case COBS_RET_ERR_EXHAUSTED:
            // printf("cobs buffer overflow error\n");
            break;
            default:
            // printf("cobs error\n");
            break;
        }
    }
}

int main()
{
    stdio_init_all();

    xQueue = xQueueCreate(1, sizeof(uint));

    xTaskCreate(adc_task, "ADC_Task", 256, NULL, 1, NULL);
    xTaskCreate(telemetry_task, "telemetry_Task", 256, NULL, 1, NULL);
    // recieve commands
    // send CAN messages
    // recieve CAN messages
    
    vTaskStartScheduler();

    while(1){};
}
