/*
 * main.c
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Zeepunt
 */
#include <stdio.h>
#include <time.h>

#include "lwip/init.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"

#include "port_netif.h"
#include "app_tcpecho.h"

#if LWIP_DHCP
static struct dhcp s_dhcp;
#elif LWIP_AUTOIP
static struct autoip s_autoip;
#endif

#if LWIP_NETIF_STATUS_CALLBACK
static void priv_status_callback(struct netif *netif)
{
    if (netif_is_up(netif)) {
        printf("status: UP\n");
    } else {
        printf("status: DOWN\n");
    }
}
#endif

#if LWIP_NETIF_LINK_CALLBACK
static void priv_link_callback(struct netif *netif)
{
    if (netif_is_link_up(netif)) {
        printf("link: UP\n");
    } else {
        printf("link: DOWN\n");
    }
}
#endif

static void priv_ipv4_init(void)
{
    err_t err = ERR_OK;

    ip4_addr_t gw;
    ip4_addr_t mask;
    ip4_addr_t addr;

    ip4_addr_set_zero(&gw);
    ip4_addr_set_zero(&mask);
    ip4_addr_set_zero(&addr);

#if 0
#if LWIP_DHCP
#elif LWIP_AUTOIP
#else
    IP4_ADDR(&gw, 192, 168, 1, 1);
    IP4_ADDR(&mask, 255, 255, 255, 0);
    IP4_ADDR(&addr, 192, 168, 1, 200);
#endif
#else
    IP4_ADDR(&gw, 192, 168, 1, 1);
    IP4_ADDR(&mask, 255, 255, 255, 0);
    IP4_ADDR(&addr, 192, 168, 1, 200);
#endif

    port_netif_init(&addr, &mask, &gw);

#if LWIP_NETIF_STATUS_CALLBACK
    netif_set_status_callback(netif_default, priv_status_callback);
#endif

#if LWIP_NETIF_LINK_CALLBACK
    netif_set_link_callback(netif_default, priv_link_callback);
#endif

#if LWIP_DHCP
    dhcp_set_struct(netif_default, &s_dhcp);
#elif LWIP_AUTOIP
    autoip_set_struct(netif_default, &s_autoip);
#endif

    netif_set_up(netif_default);

#if 0
#if LWIP_DHCP
    err = dhcp_start(netif_default);
    LWIP_ASSERT("dhcp_start failed", err == ERR_OK);
#elif LWIP_AUTOIP
    err = autoip_start(netif_default);
    LWIP_ASSERT("autoip_start failed", err == ERR_OK);
#endif
#endif

    printf("ipv4 init done\n");
}

static void priv_init_done_cb(void *arg)
{
    sys_sem_t *init_sem = NULL;
    LWIP_ASSERT("arg != NULL", arg != NULL);
    init_sem = (sys_sem_t *)arg;

    srand((unsigned int)time(NULL));

#if LWIP_IPV4
    priv_ipv4_init();
#endif

    sys_sem_signal(init_sem);
}

static void priv_lwip_thread(void *arg)
{
    printf("lwip thread enter\n");

    while (1) {
        port_netif_poll();
    }

    printf("lwip thread exit\n");
}

void lwip_app_platform_assert(const char *msg, int line, const char *file)
{
    printf("Assertion \"%s\" failed at line %d in %s\n", msg, line, file);
    fflush(NULL);
    abort();
}

int main(int argc, char *argv[])
{
    err_t err = ERR_OK;
    sys_sem_t init_sem;

    err = sys_sem_new(&init_sem, 0);
    LWIP_ASSERT("failed to create init_sem", err == ERR_OK);

    tcpip_init(priv_init_done_cb, &init_sem);

    sys_sem_wait(&init_sem);
    sys_sem_free(&init_sem);

    printf("lwip init done\n");

    sys_thread_new("lwip_thread", priv_lwip_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

    app_tcpecho_init();

    while (1) {
        int ch = getchar();
        if ((ch == 'q') || (ch == 'Q')) {
            break;
        }

        sleep(1);
    }

    port_netif_deinit();

    printf("lwip exit\n");

    return 0;
}
