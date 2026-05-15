/*
 * app_tcpecho.c
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Zeepunt
 */
#include <stdio.h>

#include "lwip/sys.h"
#include "lwip/sockets.h"

#define TCPECHO_PORT          8888
#define TCPECHO_LISTEN_NUM    5

static void priv_tcpecho_thread(void *arg)
{
    int ret = -1;
    int listen_fd = -1;
    int client_fd = -1;

    char buf[256];

    struct sockaddr_in addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;

    listen_fd = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        goto exit;
    }

    printf("tcpecho enter\n");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = IPADDR_ANY;
    addr.sin_port = htons(TCPECHO_PORT);

    ret = lwip_bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret != 0) {
        goto exit;
    }

    ret = lwip_listen(listen_fd, TCPECHO_LISTEN_NUM);
    if (ret != 0) {
        goto exit;
    }

    while (1) {
        client_len = sizeof(client_addr);
        client_fd = lwip_accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            continue;
        }

        ret = lwip_recv(client_fd, buf, sizeof(buf), 0);
        if (ret > 0) {
            lwip_send(client_fd, buf, ret, 0);
        }

        lwip_close(client_fd);
    }

exit:
    if (listen_fd >= 0) {
        lwip_close(listen_fd);
        listen_fd = -1;
    }

    printf("tcpecho exit\n");
}

void app_tcpecho_init(void)
{
    sys_thread_new("tcpecho_thread", priv_tcpecho_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
