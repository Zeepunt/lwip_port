/*
 * port_tapif.h
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Zeepunt
 */
#ifndef __PORT_TAPIF_H__
#define __PORT_TAPIF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/err.h"
#include "lwip/netif.h"

err_t port_tapif_init(struct netif *netif);

void port_tapif_poll(struct netif *netif);

void port_tapif_deinit(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif /* __PORT_TAPIF_H__ */
