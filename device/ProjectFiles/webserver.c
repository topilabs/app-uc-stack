#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "tusb.h"
#include "dhserver.h"
#include "dnserver.h"
#include "lwip/ethip6.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/tcp.h"
#include "httpd.h"
#include "httpd_opts.h"
#include "httpd_structs.h"
#include "common.h"

/* lwip context */
static struct netif netif_data;

/* shared between tud_network_recv_cb() and service_traffic() */
static struct pbuf *received_frame;

/* this is used by this code, ./class/net/net_driver.c, and usb_descriptors.c */
uint8_t tud_network_mac_address[6] = { 0x02, 0x02, 0x84, 0x6A, 0x96, 0x00 };

/* DHCP server configuration */
static dhcp_entry_t entries[3];
static dhcp_config_t dhcp_config;

/* Global IP address */
static ip4_addr_t ipaddr;

static err_t linkoutput_fn(struct netif *netif, struct pbuf *p) {
    (void) netif;
    for (;;) {
        if (!tud_ready())
            return ERR_USE;
        if (tud_network_can_xmit(p->tot_len)) {
            tud_network_xmit(p, 0);
            return ERR_OK;
        }
        tud_task();
    }
}

static err_t ip4_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *addr) {
    return etharp_output(netif, p, addr);
}

static err_t netif_init_cb(struct netif *netif) {
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    netif->mtu = CFG_TUD_NET_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    netif->state = NULL;
    netif->name[0] = 'E';
    netif->name[1] = 'X';
    netif->linkoutput = linkoutput_fn;
    netif->output = ip4_output_fn;
    return ERR_OK;
}

static void init_lwip(void) {
    struct netif *netif = &netif_data;
    ip4_addr_t netmask, gateway;
    IP4_ADDR(&ipaddr, 192, 168, 7, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gateway, 0, 0, 0, 0);

    IP4_ADDR(&entries[0].addr, 192, 168, 7, 2);
    IP4_ADDR(&entries[1].addr, 192, 168, 7, 3);
    IP4_ADDR(&entries[2].addr, 192, 168, 7, 4);
    memset(entries[0].mac, 0, sizeof(entries[0].mac));
    memset(entries[1].mac, 0, sizeof(entries[1].mac));
    memset(entries[2].mac, 0, sizeof(entries[2].mac));

    IP4_ADDR(&dhcp_config.router, 0, 0, 0, 0);
    dhcp_config.port = 67;
    IP4_ADDR(&dhcp_config.dns, 192, 168, 7, 1);
    dhcp_config.num_entry = 3;
    dhcp_config.entries = entries;

    lwip_init();
    netif->hwaddr_len = sizeof(tud_network_mac_address);
    memcpy(netif->hwaddr, tud_network_mac_address, sizeof(tud_network_mac_address));
    netif->hwaddr[5] ^= 0x01;
    netif = netif_add(netif, &ipaddr, &netmask, &gateway, NULL, netif_init_cb, ip_input);
    netif_set_default(netif);
}

bool dns_query_proc(const char *name, ip4_addr_t *addr) {
    if (0 == strcmp(name, "tiny.usb")) {
        *addr = ipaddr;
        return true;
    }
    return false;
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size) {
    if (received_frame) return false;
    if (size) {
        struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
        if (p) {
            memcpy(p->payload, src, size);
            received_frame = p;
        }
    }
    return true;
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg) {
    struct pbuf *p = (struct pbuf *) ref;
    (void) arg;
    return pbuf_copy_partial(p, dst, p->tot_len, 0);
}

static void service_traffic(void) {
    if (received_frame) {
        ethernet_input(received_frame, &netif_data);
        pbuf_free(received_frame);
        received_frame = NULL;
        tud_network_recv_renew();
    }
    sys_check_timeouts();
}

void tud_network_init_cb(void) {
    if (received_frame) {
        pbuf_free(received_frame);
        received_frame = NULL;
    }
}

void webserver_task(void *pvParameters)
{
    tud_init(BOARD_TUD_RHPORT);
    init_lwip();
    while (!netif_is_up(&netif_data));
    while (dhserv_init(&dhcp_config) != ERR_OK);
    while (dnserv_init(IP_ADDR_ANY, 53, dns_query_proc) != ERR_OK);
    httpd_init();
    while (1) {
        tud_task();
        service_traffic();
        vTaskDelay(1);
    }
} 