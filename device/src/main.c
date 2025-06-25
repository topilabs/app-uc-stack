#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <queue.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "../config/lwipopts.h"
#include "hardware/adc.h"
#include "components/nanopb/message.pb.h"

#include <pb_encode.h>
#include <pb_decode.h>

#include "components/mongoose/mongoose_config.h"
#include "components/mongoose/mongoose.h"
#include "components/mongoose/net.h"
#include "wifi.h"

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
    cyw43_arch_init();
    
    const uint LED_PIN = CYW43_WL_GPIO_LED_PIN; // Use the built-in LED pin
    const uint KNOB_PIN = 26;

    uint uIValueToSend = 0;
    // gpio_init(LED_PIN);
    // gpio_set_dir(LED_PIN, GPIO_OUT);

    adc_init();
    adc_gpio_init(KNOB_PIN);
    adc_select_input(0);
    
    while (true) {
        cyw43_arch_gpio_put(LED_PIN, true);
        uIValueToSend = adc_read();
        xQueueSend(xQueue, &uIValueToSend, 0U);
        vTaskDelay(10);

        cyw43_arch_gpio_put(LED_PIN, false);
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

    // Allocate buffer to hold the encoded message
    #define buffer_length  128
    uint8_t buffer[buffer_length];
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
        if (prep_buf(uiRecievedValue, buffer, buffer_length, &bytes_written)) {
            uart_write_blocking(UART_ID, buffer, bytes_written);   // Send the data
        }
    }
}

static void mongoose(void *args) {
  struct mg_mgr mgr;        // Initialise Mongoose event manager
  mg_mgr_init(&mgr);        // and attach it to the interface
  mg_log_set(MG_LL_DEBUG);  // Set log level

  cyw43_arch_init();
  cyw43_arch_enable_sta_mode();
  cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK);

  MG_INFO(("Initialising application..."));
  web_init(&mgr);

  MG_INFO(("Starting event loop"));
  for (;;) {
    mg_mgr_poll(&mgr, 10);
  }

  (void) args;
}


int main()
{
    stdio_init_all();

    xQueue = xQueueCreate(1, sizeof(uint));

    xTaskCreate(mongoose, "mongoose", 2048, 0, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(adc_task, "ADC_Task", 256, NULL, 1, NULL);
    xTaskCreate(telemetry_task, "telemetry_Task", 256, NULL, 1, NULL);
    // recieve commands
    // send CAN messages
    // recieve CAN messages
    
    vTaskStartScheduler();

    while(1){};
}
