#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <queue.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/unique_id.h"

// TinyUSB
#include "tusb.h"
#include "class/net/net_device.h"

// lwIP
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/timeouts.h"

// libwebsockets
#include <libwebsockets.h>

// Protocol Buffers
#include <pb_encode.h>
#include <pb_decode.h>
#include "nanopb/message.pb.h"

// Queue for data exchange between tasks
static QueueHandle_t xQueue = NULL;

// Network interface
static struct netif netif_data;

// libwebsockets context
struct lws_context *context;

// WebSocket server configuration
#define WS_PORT 9000
#define MAX_PAYLOAD 128

// USB CDC-ECM configuration
#define USBD_VID 0x2E8A  // Raspberry Pi
#define USBD_PID 0x000A  // Example PID
#define USBD_MANUFACTURER "Raspberry Pi"
#define USBD_PRODUCT "Pico WebSocket Server"
#define USBD_SERIAL "123456"

// Static IP configuration
#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 7
#define IP_ADDR3 1

#define NETMASK_ADDR0 255
#define NETMASK_ADDR1 255
#define NETMASK_ADDR2 255
#define NETMASK_ADDR3 0

#define GW_ADDR0 192
#define GW_ADDR1 168
#define GW_ADDR2 7
#define GW_ADDR3 1

// Function prototypes
void adc_task(void *pvParameters);
void network_task(void *pvParameters);
void websocket_task(void *pvParameters);
int prep_protobuf(uint value, uint8_t* buf, size_t buf_len, size_t* bytes_written);

// WebSocket protocol definition
static int callback_pico_ws(struct lws *wsi, enum lws_callback_reasons reason,
                           void *user, void *in, size_t len);

static const struct lws_protocols protocols[] = {
    {
        "pico-ws-protocol",
        callback_pico_ws,
        0,  // per_session_data_size
        MAX_PAYLOAD,
    },
    { NULL, NULL, 0, 0 }  // terminator
};

// Global variables for WebSocket communication
static uint8_t ws_buffer[LWS_PRE + MAX_PAYLOAD];
static int ws_data_ready = 0;
static size_t ws_data_len = 0;

// ADC task - reads potentiometer values
void adc_task(void *pvParameters)
{   
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
int prep_protobuf(uint value, uint8_t* buf, size_t buf_len, size_t* bytes_written)
{
    DataPackage message = DataPackage_init_default;
    message.potentiometer = value;
    message.generator = 3;

    // Encode the message
    pb_ostream_t stream = pb_ostream_from_buffer(buf, buf_len);
    if (!pb_encode(&stream, DataPackage_fields, &message)) {
        printf("Error encoding protobuf\n");
        return 0;
    }
    
    *bytes_written = stream.bytes_written;
    return 1;
}

// WebSocket callback function
static int callback_pico_ws(struct lws *wsi, enum lws_callback_reasons reason,
                           void *user, void *in, size_t len)
{
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            printf("WebSocket connection established\n");
            lws_callback_on_writable(wsi);
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            if (ws_data_ready) {
                int m = lws_write(wsi, &ws_buffer[LWS_PRE], ws_data_len, LWS_WRITE_BINARY);
                if (m < (int)ws_data_len) {
                    printf("Error writing to WebSocket\n");
                    return -1;
                }
                ws_data_ready = 0;
                // Schedule next write if there's more data
                if (uxQueueMessagesWaiting(xQueue) > 0) {
                    lws_callback_on_writable(wsi);
                }
            }
            break;

        case LWS_CALLBACK_RECEIVE:
            printf("Received data: %.*s\n", (int)len, (char *)in);
            // Handle incoming messages if needed
            break;

        case LWS_CALLBACK_CLOSED:
            printf("WebSocket connection closed\n");
            break;

        default:
            break;
    }

    return 0;
}

// Network initialization
static err_t netif_init_fn(struct netif *netif)
{
    return ERR_OK;
}

// Network task - handles lwIP processing
void network_task(void *pvParameters)
{
    // Initialize lwIP
    lwip_init();

    // Initialize network interface
    ip4_addr_t ipaddr, netmask, gateway;
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gateway, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    netif_add(&netif_data, &ipaddr, &netmask, &gateway, NULL, netif_init_fn, ip_input);
    netif_set_default(&netif_data);
    netif_set_up(&netif_data);

    printf("Network initialized with IP: %d.%d.%d.%d\n", 
           IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);

    // Main network processing loop
    while (1) {
        // Process lwIP timers
        sys_check_timeouts();
        
        // Give other tasks a chance to run
        vTaskDelay(1);
    }
}

// WebSocket task - handles WebSocket server and data transmission
void websocket_task(void *pvParameters)
{
    // Initialize libwebsockets
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = WS_PORT;
    info.protocols = protocols;
    info.vhost_name = "pico-ws";
    info.options = LWS_SERVER_OPTION_EXPLICIT_VHOSTS |
                   LWS_SERVER_OPTION_VALIDATE_UTF8 |
                   LWS_SERVER_OPTION_SKIP_SERVER_CANONICAL_NAME;
    
    // Memory constraints for Pico
    info.pt_serv_buf_size = 512;
    info.max_http_header_data = 512;
    info.max_http_header_pool = 4;
    info.count_threads = 1;
    info.fd_limit_per_thread = 6;  // Limit concurrent connections

    context = lws_create_context(&info);
    if (!context) {
        printf("libwebsockets init failed\n");
        vTaskDelete(NULL);
        return;
    }

    printf("WebSocket server started on port %d\n", WS_PORT);

    uint uiReceivedValue;
    size_t bytes_written;

    // Main WebSocket processing loop
    while (1) {
        // Service WebSocket connections
        lws_service(context, 0);
        
        // Check for new data from ADC
        if (!ws_data_ready && xQueueReceive(xQueue, &uiReceivedValue, 0) == pdTRUE) {
            // Encode data as Protocol Buffer
            if (prep_protobuf(uiReceivedValue, &ws_buffer[LWS_PRE], MAX_PAYLOAD, &bytes_written)) {
                ws_data_len = bytes_written;
                ws_data_ready = 1;
                // Send to all connected WebSocket clients
                lws_callback_on_writable_all_protocol(context, &protocols[0]);
            }
        }
        
        // Give other tasks a chance to run
        vTaskDelay(1);
    }
}

// USB device callbacks
void tud_network_init_cb(void)
{
    // Initialize network interface for TinyUSB
    // This will be called when the USB device is initialized
    printf("TinyUSB network initialized\n");
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
    // Handle incoming Ethernet frames
    // Forward to lwIP stack
    struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
    if (p) {
        memcpy(p->payload, src, size);
        netif_data.input(p, &netif_data);
    }
    return true;
}

// USB descriptors
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = USBD_VID,
    .idProduct          = USBD_PID,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

// USB string descriptors
char const *string_desc_arr[] = {
    (const char[]) { 0x09, 0x04 },  // 0: is supported language is English (0x0409)
    USBD_MANUFACTURER,              // 1: Manufacturer
    USBD_PRODUCT,                   // 2: Product
    USBD_SERIAL,                    // 3: Serial
};

// Main function
int main()
{
    stdio_init_all();
    
    printf("Pico WebSocket Server starting...\n");
    
    // Initialize USB stack
    tusb_init();
    
    // Create queue for data exchange
    xQueue = xQueueCreate(10, sizeof(uint));
    
    // Create tasks
    xTaskCreate(adc_task, "ADC_Task", 256, NULL, 1, NULL);
    xTaskCreate(network_task, "Network_Task", 1024, NULL, 2, NULL);
    xTaskCreate(websocket_task, "WebSocket_Task", 2048, NULL, 2, NULL);
    
    // Start the scheduler
    vTaskStartScheduler();

    // Should never reach here
    while(1){};
}
