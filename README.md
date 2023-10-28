## 一、移植参考

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
# 1. 第 19 行的 /* #define USE_DHCP    0 */
#define USE_DHCP 0
# 2. 第 20 行的 /* #define USE_AUTOIP  0 */
#define USE_AUTOIP 0
# 3. 第 58 行的 LWIP_PING_APP
#define LWIP_PING_APP                 1

# 修改文件的选项
vim ./contrib/examples/example_app/lwipopts.h
# 1. 第 40 行的 LWIP_IPV6
#define LWIP_IPV6                  0
# 2. 第 77 行定义一个宏 LWIP_DEBUG
#define LWIP_DEBUG

cd ./contrib/ports/unix/example_app

cmake -B build -DLWIP_DIR=../../../../
cmake --build build
sudo ./build/example_app
```

