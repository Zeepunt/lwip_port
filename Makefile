Q := @

BUILD_DIR := build
LWIP_DIR := lwip
PORT_DIR := port

TARGET = $(BUILD_DIR)/main

CC_PREFIX :=
AS        := $(CC_PREFIX)as
CC        := $(CC_PREFIX)gcc
CPP       := $(CC_PREFIX)g++
LD        := $(CC_PREFIX)ld
NM        := $(CC_PREFIX)nm
AR        := $(CC_PREFIX)ar
OBJCOPY   := $(CC_PREFIX)objcopy
OBJDUMP   := $(CC_PREFIX)objdump
SIZE      := $(CC_PREFIX)size
STRIP     := $(CC_PREFIX)strip

C_FLAGS := -Wall -g -O0

LWIP_INCS := \
	-I${LWIP_DIR}/src/include

LWIP_CORE_SRCS := \
	${LWIP_DIR}/src/core/init.c \
	${LWIP_DIR}/src/core/def.c \
	${LWIP_DIR}/src/core/dns.c \
	${LWIP_DIR}/src/core/inet_chksum.c \
	${LWIP_DIR}/src/core/ip.c \
	${LWIP_DIR}/src/core/mem.c \
	${LWIP_DIR}/src/core/memp.c \
	${LWIP_DIR}/src/core/netif.c \
	${LWIP_DIR}/src/core/pbuf.c \
	${LWIP_DIR}/src/core/raw.c \
	${LWIP_DIR}/src/core/stats.c \
	${LWIP_DIR}/src/core/sys.c \
	${LWIP_DIR}/src/core/altcp.c \
	${LWIP_DIR}/src/core/altcp_alloc.c \
	${LWIP_DIR}/src/core/altcp_tcp.c \
	${LWIP_DIR}/src/core/tcp.c \
	${LWIP_DIR}/src/core/tcp_in.c \
	${LWIP_DIR}/src/core/tcp_out.c \
	${LWIP_DIR}/src/core/timeouts.c \
	${LWIP_DIR}/src/core/udp.c

LWIP_CORE4_SRCS := \
	${LWIP_DIR}/src/core/ipv4/acd.c \
	${LWIP_DIR}/src/core/ipv4/autoip.c \
	${LWIP_DIR}/src/core/ipv4/dhcp.c \
	${LWIP_DIR}/src/core/ipv4/etharp.c \
	${LWIP_DIR}/src/core/ipv4/icmp.c \
	${LWIP_DIR}/src/core/ipv4/igmp.c \
	${LWIP_DIR}/src/core/ipv4/ip4_frag.c \
	${LWIP_DIR}/src/core/ipv4/ip4.c \
	${LWIP_DIR}/src/core/ipv4/ip4_addr.c

LWIP_CORE6_SRCS := \
	${LWIP_DIR}/src/core/ipv6/dhcp6.c \
	${LWIP_DIR}/src/core/ipv6/ethip6.c \
	${LWIP_DIR}/src/core/ipv6/icmp6.c \
	${LWIP_DIR}/src/core/ipv6/inet6.c \
	${LWIP_DIR}/src/core/ipv6/ip6.c \
	${LWIP_DIR}/src/core/ipv6/ip6_addr.c \
	${LWIP_DIR}/src/core/ipv6/ip6_frag.c \
	${LWIP_DIR}/src/core/ipv6/mld6.c \
	${LWIP_DIR}/src/core/ipv6/nd6.c

LWIP_API_SRCS := \
	${LWIP_DIR}/src/api/api_lib.c \
	${LWIP_DIR}/src/api/api_msg.c \
	${LWIP_DIR}/src/api/err.c \
	${LWIP_DIR}/src/api/if_api.c \
	${LWIP_DIR}/src/api/netbuf.c \
	${LWIP_DIR}/src/api/netdb.c \
	${LWIP_DIR}/src/api/netifapi.c \
	${LWIP_DIR}/src/api/sockets.c \
	${LWIP_DIR}/src/api/tcpip.c

LWIP_NETIF_SRCS := \
	${LWIP_DIR}/src/netif/ethernet.c \
	${LWIP_DIR}/src/netif/bridgeif.c \
	${LWIP_DIR}/src/netif/bridgeif_fdb.c

LWIP_PPP_SRCS := \
	${LWIP_DIR}/src/netif/ppp/auth.c \
	${LWIP_DIR}/src/netif/ppp/ccp.c \
	${LWIP_DIR}/src/netif/ppp/chap-md5.c \
	${LWIP_DIR}/src/netif/ppp/chap_ms.c \
	${LWIP_DIR}/src/netif/ppp/chap-new.c \
	${LWIP_DIR}/src/netif/ppp/demand.c \
	${LWIP_DIR}/src/netif/ppp/eap.c \
	${LWIP_DIR}/src/netif/ppp/ecp.c \
	${LWIP_DIR}/src/netif/ppp/eui64.c \
	${LWIP_DIR}/src/netif/ppp/fsm.c \
	${LWIP_DIR}/src/netif/ppp/ipcp.c \
	${LWIP_DIR}/src/netif/ppp/ipv6cp.c \
	${LWIP_DIR}/src/netif/ppp/lcp.c \
	${LWIP_DIR}/src/netif/ppp/magic.c \
	${LWIP_DIR}/src/netif/ppp/mppe.c \
	${LWIP_DIR}/src/netif/ppp/multilink.c \
	${LWIP_DIR}/src/netif/ppp/ppp.c \
	${LWIP_DIR}/src/netif/ppp/pppapi.c \
	${LWIP_DIR}/src/netif/ppp/pppcrypt.c \
	${LWIP_DIR}/src/netif/ppp/pppoe.c \
	${LWIP_DIR}/src/netif/ppp/pppol2tp.c \
	${LWIP_DIR}/src/netif/ppp/pppos.c \
	${LWIP_DIR}/src/netif/ppp/upap.c \
	${LWIP_DIR}/src/netif/ppp/utils.c \
	${LWIP_DIR}/src/netif/ppp/vj.c \
	${LWIP_DIR}/src/netif/ppp/polarssl/arc4.c \
	${LWIP_DIR}/src/netif/ppp/polarssl/des.c \
	${LWIP_DIR}/src/netif/ppp/polarssl/md4.c \
	${LWIP_DIR}/src/netif/ppp/polarssl/md5.c \
	${LWIP_DIR}/src/netif/ppp/polarssl/sha1.c

LWIP_PORT_INCS := \
	-I${LWIP_DIR}/contrib/ports/unix/port/include

LWIP_PORT_UNIX_SRCS := \
	${LWIP_DIR}/contrib/ports/unix/port/sys_arch.c \
	${LWIP_DIR}/contrib/ports/unix/port/perf.c \
	${LWIP_DIR}/contrib/ports/unix/port/netif/tapif.c \
	${LWIP_DIR}/contrib/ports/unix/port/netif/list.c \
	${LWIP_DIR}/contrib/ports/unix/port/netif/sio.c \
	${LWIP_DIR}/contrib/ports/unix/port/netif/fifo.c

PORT_INCS := \
	-I${PORT_DIR}/inc

PORT_SRCS := \
	${PORT_DIR}/src/main.c \
	${PORT_DIR}/src/port_tapif.c \
	${PORT_DIR}/src/port_netif.c \
	${PORT_DIR}/src/app_tcpecho.c

C_INCLUDES := \
	$(LWIP_INCS) \
	$(LWIP_PORT_INCS) \
	$(PORT_INCS)

C_SOURCES := \
	$(LWIP_CORE_SRCS) \
	$(LWIP_CORE4_SRCS) \
	$(LWIP_CORE6_SRCS) \
	$(LWIP_API_SRCS) \
	$(LWIP_NETIF_SRCS) \
	$(LWIP_PPP_SRCS) \
	$(LWIP_PORT_UNIX_SRCS) \
	$(PORT_SRCS)

C_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

$(BUILD_DIR)/%.o : %.c
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) -c $(C_FLAGS) $(C_INCLUDES) -o $@ $<

$(TARGET): $(C_OBJECTS)
	$(Q)$(CC) $^ -o $@
	$(Q)$(SIZE) $@

PHONY :=
PHONY += all
all: init $(TARGET)

PHONY += init
env:
	$(Q)if [ ! -d $(BUILD_DIR) ]; then \
		mkdir -p $(BUILD_DIR); \
	fi

PHONY += clean
clean:
	$(Q)rm -rf $(BUILD_DIR)

PHONY += run
run:
	$(Q)$(TARGET)

.PHONY: $(PHONY)
