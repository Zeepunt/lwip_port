# 一、官方移植参考

lwip 官方有提供 linux 下的移植项目，代码路径为：`lwip/contrib/ports/unix/`。

可直接在该目录下编译执行：

```shell
# 安装依赖
sudo apt install bridge-utils

cd lwip

# 在 Linux 上 lwip 默认使用 tap0 
sudo ./contrib/ports/unix/setup-tapif

cp ./contrib/examples/example_app/lwipcfg.h.example ./contrib/examples/example_app/lwipcfg.h

# 修改文件的配置
vim ./contrib/examples/example_app/lwipcfg.h
#define USE_DHCP 0
#define USE_AUTOIP 0
#define LWIP_PING_APP                 1

# 修改文件的选项
vim ./contrib/examples/example_app/lwipopts.h
#define LWIP_IPV6                  0
#define LWIP_DEBUG

cd ./contrib/ports/unix/example_app

cmake -B build -DLWIP_DIR=../../../../
cmake --build build
sudo ./build/example_app
```

# 自定义移植

## 功能框图

```shell
     Lwip
+------------+              +-----------------------------------+
|  +-------+ | /dev/net/tun | +------+  bridge  +-------------+ |
|  | tapif |-+--------------+-| tap0 | <------> | lwip_bridge | |
|  +-------+ |              | +------+          +-------------+ |
+------------+              |     |                    |        |
                            |     +----------+---------+        |
                            |                |                  |
                            |     +----------+---------+        |
                            |     | Host (192.168.1.1) |        |
                            |     +--------------------+        |
                            +-----------------------------------+
```

1. tapif 使用 /dev/net/tun 创建一个 tap 设备, 比如 tap0

   > - /dev/net/tun 即 tunnel
   > - 创建的是 tap 设备, 处理的是以太网帧

2. 创建一个名为 lwip_bridge 的网桥设备, 然后将 tap 设备桥接到 lwip_bridge

   > 网桥设备是一个以太网交换机
   > 当给网桥设备分配一个和主机位于同一网段的 IP 地址后, 它可以将从 Lwip 收到的以太网帧转发给主机

## 操作

```shell
# 编译
make

# 清理
make clean

# 运行
make run
```
