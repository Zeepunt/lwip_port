/*
 * port_netif.c
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Zeepunt
 */
#include "lwip/sys.h"
#include "lwip/tcpip.h"

#include "port_tapif.h"
#include "port_netif.h"

static struct netif s_netif;

void port_netif_init(const ip4_addr_t *addr, const ip4_addr_t *mask, const ip4_addr_t *gw)
{
    netif_add(&s_netif, addr, mask, gw, NULL, port_tapif_init, tcpip_input);

    netif_set_default(&s_netif);
}

void port_netif_poll(void)
{
    port_tapif_poll(&s_netif);
}

void port_netif_deinit(void)
{
    port_tapif_deinit(&s_netif);
}
