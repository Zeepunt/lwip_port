/*
 * port_tapif.c
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Zeepunt
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/ethip6.h"
#include "lwip/snmp.h"

#include "port_tapif.h"

#define TAP_DEV_NAME    "tap0"

#define BRIDGE_DEV_NAME "br0"
#define BRIDGE_DEV_IP   "192.168.1.1/24"

static int s_tap_fd = -1;

static int priv_run_cmd(const char *fmt, ...)
{
    int ret = 0;
    char cmd[256] = {0};

    va_list args;
    va_start(args, fmt);
    vsnprintf(cmd, sizeof(cmd), fmt, args);
    va_end(args);

    ret = system(cmd);
    if (ret != 0) {
        printf("cmd (%s) run failed: %d\n", cmd, ret);
    }

    return ret;
}

static int priv_tap_dev_init(int tap_fd)
{
    int ret = 0;
    struct ifreq ifr; /* Interface Request */

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strncpy(ifr.ifr_name, TAP_DEV_NAME, IFNAMSIZ);
    ret = ioctl(tap_fd, TUNSETIFF, (void *)&ifr);
    if (ret < 0) {
        perror("ioctl TUNSETIFF failed");
        return -1;
    }

    ret = priv_run_cmd("ip link set %s up", TAP_DEV_NAME);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

static void priv_tap_dev_deinit(void)
{
    (void)priv_run_cmd("ip link set %s down", TAP_DEV_NAME);
    (void)priv_run_cmd("ip link del %s", TAP_DEV_NAME);
}

static int priv_bridge_dev_init(void)
{
    int ret = 0;

    ret = priv_run_cmd("ip link add %s type bridge", BRIDGE_DEV_NAME);
    if (ret != 0) {
        return -1;
    }

    ret = priv_run_cmd("ip link set %s master %s", TAP_DEV_NAME, BRIDGE_DEV_NAME);
    if (ret != 0) {
        return -1;
    }

    ret = priv_run_cmd("ip addr add %s dev %s", BRIDGE_DEV_IP, BRIDGE_DEV_NAME);
    if (ret != 0) {
        return -1;
    }

    ret = priv_run_cmd("ip link set %s up", BRIDGE_DEV_NAME);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

static void priv_bridge_dev_deinit(void)
{
    (void)priv_run_cmd("ip link set %s nomaster", TAP_DEV_NAME);
    (void)priv_run_cmd("ip link del %s", BRIDGE_DEV_NAME);
}

static int priv_tunnel_init(void)
{
    int ret = 0;

    s_tap_fd = open("/dev/net/tun", O_RDWR);
    if (s_tap_fd < 0) {
        perror("open tun failed");
        ret = -1;
        goto exit;
    }

    ret = priv_tap_dev_init(s_tap_fd);
    if (ret < 0) {
        ret = -1;
        goto exit;
    }

    ret = priv_bridge_dev_init();
    if (ret < 0) {
        ret = -1;
        goto exit;
    }

    printf("TAP interface %s added to bridge %s with IP %s\n", TAP_DEV_NAME, BRIDGE_DEV_NAME, BRIDGE_DEV_IP);

    return 0;

exit:
    if (s_tap_fd >= 0) {
        close(s_tap_fd);
        s_tap_fd = -1;
    }
    return ret;
}

static void priv_tunnel_deinit(void)
{
    priv_bridge_dev_deinit();
    priv_tap_dev_deinit();

    if (s_tap_fd >= 0) {
        close(s_tap_fd);
        s_tap_fd = -1;
    }
}

static void priv_low_level_init(struct netif *netif)
{
    int ret = 0;

    /**
     * MAC 地址是 12:34:56:78:9a:bc
     * 第一个字节的 bit[0] = 0 表示单播, = 1 表示组播
     */
    netif->hwaddr[0] = 0x12;
    netif->hwaddr[1] = 0x34;
    netif->hwaddr[2] = 0x56;
    netif->hwaddr[3] = 0x78;
    netif->hwaddr[4] = 0x9a;
    netif->hwaddr[5] = 0xbc;
    netif->hwaddr_len = 6;

    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

    ret = priv_tunnel_init();
    LWIP_ASSERT("tunnel init failed", ret == 0);

    netif_set_link_up(netif);
}

static err_t priv_low_level_output(struct netif *netif, struct pbuf *p)
{
    char buf[1518]; /* max packet size including VLAN excluding CRC */

    ssize_t written;

    if (p->tot_len > sizeof(buf)) {
        MIB2_STATS_NETIF_INC(netif, ifoutdiscards);
        printf("tapif: packet too large");
        return ERR_IF;
    }

    pbuf_copy_partial(p, buf, p->tot_len, 0);

    written = write(s_tap_fd, buf, p->tot_len);
    if (written < p->tot_len) {
        MIB2_STATS_NETIF_INC(netif, ifoutdiscards);
        printf("tapif: write error");
        return ERR_IF;
    } else {
        MIB2_STATS_NETIF_ADD(netif, ifoutoctets, (u32_t)written);
        return ERR_OK;
    }
}

static struct pbuf *priv_low_level_input(struct netif *netif)
{
    struct pbuf *p;
    u16_t len;
    ssize_t readlen;

    char buf[1518]; /* max packet size including VLAN excluding CRC */

    readlen = read(s_tap_fd, buf, sizeof(buf));
    if (readlen < 0) {
        perror("read returned -1");
        exit(1);
    }
    len = (u16_t)readlen;

    MIB2_STATS_NETIF_ADD(netif, ifinoctets, len);

    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if (p != NULL) {
        pbuf_take(p, buf, len);
    } else {
        MIB2_STATS_NETIF_INC(netif, ifindiscards);
        LWIP_DEBUGF(NETIF_DEBUG, ("tapif_input: could not allocate pbuf\n"));
    }

    return p;
}

static void priv_tapif_input(struct netif *netif)
{
    struct pbuf *p = priv_low_level_input(netif);

    if (p == NULL) {
#if LINK_STATS
        LINK_STATS_INC(link.recv);
#endif
        LWIP_DEBUGF(NETIF_DEBUG, ("tapif_input: low_level_input returned NULL\n"));
        return;
    }

    if (netif->input(p, netif) != ERR_OK) {
        LWIP_DEBUGF(NETIF_DEBUG, ("tapif_input: netif input error\n"));
        pbuf_free(p);
    }
}

err_t port_tapif_init(struct netif *netif)
{
    netif->name[0] = 't';
    netif->name[1] = 'p';

    MIB2_INIT_NETIF(netif, snmp_ifType_other, 100000000);

#if LWIP_IPV4
    netif->output = etharp_output;
#endif

#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif

    netif->linkoutput = priv_low_level_output;
    netif->mtu = 1500;

    priv_low_level_init(netif);

    return ERR_OK;
}

void port_tapif_poll(struct netif *netif)
{
    int ret = 0;
    fd_set fdset;

    FD_ZERO(&fdset);
    FD_SET(s_tap_fd, &fdset);

    ret = select(s_tap_fd + 1, &fdset, NULL, NULL, NULL);

    if (ret == 1) {
        LOCK_TCPIP_CORE();
        priv_tapif_input(netif);
        UNLOCK_TCPIP_CORE();
    } else if (ret == -1) {
        printf("tapif select failed\n");
    }
}

void port_tapif_deinit(struct netif *netif)
{
    priv_tunnel_deinit();
}
