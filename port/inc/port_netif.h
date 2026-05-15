/*
 * port_netif.h
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Zeepunt
 */
#ifndef __PORT_NETIF_H__
#define __PORT_NETIF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/ip_addr.h"

void port_netif_init(const ip4_addr_t *addr, const ip4_addr_t *mask, const ip4_addr_t *gw);

void port_netif_poll(void);

void port_netif_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __PORT_NETIF_H__ */
