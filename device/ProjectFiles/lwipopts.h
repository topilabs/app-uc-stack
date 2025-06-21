/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt
 *
 */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/* Prevent having to link sys_arch.c (we don't test the API layers in unit tests) */
#define NO_SYS                          0
#define MEM_ALIGNMENT                   4
#define LWIP_RAW                        1
#define LWIP_NETCONN                    1
#define LWIP_SOCKET                     1
#define LWIP_DHCP                       1
#define LWIP_ICMP                       1
#define LWIP_UDP                        1
#define LWIP_TCP                        1
#define LWIP_IPV4                       1
#define LWIP_IPV6                       0
#define ETH_PAD_SIZE                    0
#define LWIP_IP_ACCEPT_UDP_PORT(p)      ((p) == PP_NTOHS(67))

/* FreeRTOS specific options */
#define LWIP_FREERTOS_CHECK_CORE_LOCKING 1
#define LWIP_FREERTOS_SYS_ARCH_PROTECT  1
#define LWIP_FREERTOS_SYS_ARCH_DECL_PROTECT(lev) sys_prot_t lev
#define LWIP_FREERTOS_SYS_ARCH_PROTECT_IMPL(lev) lev = sys_arch_protect()
#define LWIP_FREERTOS_SYS_ARCH_UNPROTECT_IMPL(lev) sys_arch_unprotect(lev)

/* Memory options */
#define MEM_SIZE                        (20*1024)
#define MEMP_NUM_PBUF                   16
#define MEMP_NUM_UDP_PCB                4
#define MEMP_NUM_TCP_PCB                5
#define MEMP_NUM_TCP_PCB_LISTEN         8
#define MEMP_NUM_TCP_SEG                16
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 8)

/* TCP options */
#define TCP_MSS                         (1500 /*mtu*/ - 20 /*iphdr*/ - 20 /*tcphhr*/)
#define TCP_SND_BUF                     (4 * TCP_MSS)
#define TCP_WND                         (4 * TCP_MSS)
#define TCP_SND_QUEUELEN                16
#define TCP_LISTEN_BACKLOG              1

/* ARP options */
#define ETHARP_SUPPORT_STATIC_ENTRIES   1
#define LWIP_ARP                        1

/* HTTP server options */
#define LWIP_HTTPD_CGI                  1
#define LWIP_HTTPD_SSI                  1
#define LWIP_HTTPD_SSI_INCLUDE_TAG      1

/* Network interface options */
#define LWIP_SINGLE_NETIF               1
#define LWIP_NETIF_HOSTNAME             1
#define LWIP_NETIF_API                  1

/* Buffer options */
#define PBUF_POOL_SIZE                  16
#define PBUF_POOL_BUFSIZE               LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_ENCAPSULATION_HLEN+PBUF_LINK_HLEN)

/* Debug options */
#define LWIP_DEBUG                      1
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ALL
#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON

/* Other options */
#define LWIP_MULTICAST_PING             1
#define LWIP_BROADCAST_PING             1
#define LWIP_IPV6_MLD                   0
#define LWIP_IPV6_SEND_ROUTER_SOLICIT   0

#endif /* __LWIPOPTS_H__ */
