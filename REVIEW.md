# ESP32 开发复盘

## 项目概述

在 WSL (Ubuntu) + VSCode + PlatformIO 环境下搭建 ESP32 开发环境，从零开始点亮板载 LED，为后续 LCD 驱动开发做准备。

---

## 问题及解决

### 1. PlatformIO 安装与 Python 环境

**问题：** WSL 系统 Python 3.14 没有 pip，`python3 -m pip` 报错。

**解决：**
```bash
# 安装 pip
curl -sS https://bootstrap.pypa.io/get-pip.py -o /tmp/get-pip.py
python3 /tmp/get-pip.py --user --break-system-packages

# 安装 PlatformIO
export PATH="$HOME/.local/bin:$PATH"
pip install platformio --break-system-packages
```

**教训：** WSL 中 Python 3.14 比较新，需要手动处理 pip 安装。VSCode PlatformIO 扩展可能报 Python 版本警告，但命令行 `pio` 工作正常，可忽略。

---

### 2. VSCode 远端开发模式

**问题：** VSCode 左下角没有显示 "WSL: Ubuntu" 标签。

**分析：** 用户是在 Windows 原生 VSCode 中直接打开了 WSL 路径，不是通过 Remote-WSL 扩展连接。但这不影响开发工作，编译烧录都正常。

---

### 3. Flash 烧录地址错误 ✨ 核心问题 ✨

**问题：** 烧录后 ESP32 没有任何反应（LED 不亮、串口无输出）。

**根因分析：** 第一次使用 `esptool.py write_flash 0x0 firmware.bin` 只写入了固件到地址 0x0，但 ESP32 的 Flash 布局要求：

| 地址 | 内容 | 大小 |
|---|---|---|
| `0x1000` | bootloader | ~17KB |
| `0x8000` | 分区表 (partitions) | ~3KB |
| `0x10000` | 应用程序 (firmware) | ~270KB |

只烧录 `0x0` 会导致 bootloader 找不到分区表和应用程序，芯片无法启动。

**串口诊断输出（波特率 115200）：**
```
ets Jul 29 2019 12:21:46
rst:0x10 (RTCWDT_RTC_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
invalid header: 0x61736964
```
其中 `invalid header` 就是 Flash 内容损坏的典型标志。

**解决：** 使用正确地址烧录三个文件：
```bash
esptool.py --port COM3 --baud 115200 \
  write-flash 0x1000 bootloader.bin \
              0x8000 partitions.bin \
              0x10000 firmware.bin
```

PlatformIO 自带的 `pio run --target upload` 会自动处理这个布局，但因为我们是从 WSL 编译、Windows 烧录（跨环境），手动调用 esptool 时必须指定正确地址。

---

### 4. Board 配置

**问题：** 新项目使用 `board = esp32dev`，但旧的可用项目使用 `board = featheresp32`。

**分析：** 两者虽然核心都是 ESP32，但 `featheresp32`（Adafruit Feather ESP32）在某些 Flash 配置和复位时序上略有不同。使用用户确认能跑的配置 `featheresp32` 后 LED 正常工作。

---

## 工作流总结

```
WSL 编译                   Windows 烧录
┌─────────────┐            ┌──────────────┐
│ pio run      │ ───cp──→  │ esptool.py    │ ──USB──→ ESP32
│ firmware.bin │   D:盘    │ write-flash   │
└─────────────┘            └──────────────┘
```

由于 WSL 不能直接访问 Windows 的 USB 串口（COM3），需要：
1. 在 WSL 中编译（`pio run`）
2. 将生成的 bin 文件复制到 Windows 盘（`/mnt/d/`）
3. 用 Windows 侧的 esptool.py 通过 COM3 烧录

---

## 开发环境

| 组件 | 版本/型号 |
|---|---|
| MCU | ESP32-D0WD-V3 (rev v3.1) |
| 调试器 | CP210x USB to UART (COM3) |
| 板载 LED | GPIO2 |
| WSL | Ubuntu (Linux 6.6.114.1) |
| VSCode | 1.126.0 |
| PlatformIO | Core 6.1.19 |
| ESP32 框架 | Arduino (3.20017.241212) |
| 烧录工具 | esptool.py 5.3.1 |

---

## 经验教训

1. **烧录三个文件** — ESP32 需要 bootloader、分区表、应用程序分开烧录，不能只写 firmware
2. **串口诊断利器** — 115200 波特率下 ESP32 会输出启动信息，`invalid header` 就是 Flash 问题的明确信号
3. **用已知能跑的配置** — 使用旧项目中验证过的 `board = featheresp32` 配置，避免 board 差异带来的干扰
4. **跨环境烧录** — WSL 编译 + Windows 烧录是可行的，关键是文件传输和地址参数要正确
